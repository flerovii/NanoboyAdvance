/*
 * Copyright (C) 2021 fleroviux
 *
 * Licensed under GPLv3 or any later version.
 * Refer to the included LICENSE file.
 */

#include "cpu.hpp"

#include <nba/common/compiler.hpp>
#include <common/crc32.hpp>
#include <nba/log.hpp>
#include <cstring>

namespace nba::core {

using Key = InputDevice::Key;

CPU::CPU(std::shared_ptr<Config> config)
    : config(config)
    , cpu(scheduler, bus)
    , irq(cpu, scheduler)
    , dma(bus, irq, scheduler)
    , apu(scheduler, dma, bus, config)
    , ppu(scheduler, irq, dma, config)
    , timer(scheduler, irq, apu)
    , keypad(irq, config)
    , bus(scheduler, {cpu, irq, dma, apu, ppu, timer, keypad}) {
  Reset();
}

void CPU::Reset() {
  scheduler.Reset();
  cpu.Reset();
  irq.Reset();
  dma.Reset();
  timer.Reset();
  apu.Reset();
  ppu.Reset();
  bus.Reset();
  keypad.Reset();

  if (config->skip_bios) {
    cpu.SwitchMode(arm::MODE_SYS);
    cpu.state.bank[arm::BANK_SVC][arm::BANK_R13] = 0x03007FE0;
    cpu.state.bank[arm::BANK_IRQ][arm::BANK_R13] = 0x03007FA0;
    cpu.state.r13 = 0x03007F00;
    cpu.state.r15 = 0x08000000;
  }

  mp2k_soundmain_address = 0xFFFFFFFF;
  if (config->audio.mp2k_hle_enable) {
    MP2KSearchSoundMainRAM();
    apu.GetMP2K().UseCubicFilter() = config->audio.mp2k_hle_cubic;
  }
}

void CPU::Run(int cycles) {
  using HaltControl = Bus::Hardware::HaltControl;

  auto limit = scheduler.GetTimestampNow() + cycles;

  while (scheduler.GetTimestampNow() < limit) {
    if (unlikely(bus.hw.haltcnt == HaltControl::Halt && irq.HasServableIRQ())) {
      bus.hw.haltcnt = HaltControl::Run;
    }

    if (likely(bus.hw.haltcnt == HaltControl::Run)) {
      if (cpu.state.r15 == mp2k_soundmain_address) {
        MP2KOnSoundMainRAMCalled();  
      }
      cpu.Run();
    } else {
      bus.Step(scheduler.GetRemainingCycleCount());
    }
  }
}

void CPU::MP2KSearchSoundMainRAM() {
  static constexpr u32 kSoundMainCRC32 = 0x27EA7FCF;
  static constexpr int kSoundMainLength = 48;

  auto& rom = bus.memory.rom;
  auto& rom_data = rom.GetRawROM();
  u32 address_max = rom_data.size() - kSoundMainLength;

  if (address_max > rom_data.size()) {
    return;
  }

  for (u32 address = 0; address <= address_max; address += 2) {
    auto crc = crc32(&rom_data[address], kSoundMainLength);

    if (crc == kSoundMainCRC32) {
      /* We have found SoundMain().
       * The pointer to SoundMainRAM() is stored at offset 0x74.
       */
      mp2k_soundmain_address = read<u32>(rom_data.data(), address + 0x74);

      Log<Trace>("MP2K: found SoundMainRAM() routine at 0x{0:08X}.", mp2k_soundmain_address);

      if (mp2k_soundmain_address & 1) {
        mp2k_soundmain_address &= ~1;
        mp2k_soundmain_address += sizeof(u16) * 2;
      } else {
        mp2k_soundmain_address &= ~3;
        mp2k_soundmain_address += sizeof(u32) * 2;
      }

      return;
    }
  }
}

void CPU::MP2KOnSoundMainRAMCalled() {
  apu.GetMP2K().SoundMainRAM(
    *bus.GetHostAddress<MP2K::SoundInfo>(
      *bus.GetHostAddress<u32>(0x0300'7FF0)
    )
  );
}

} // namespace nba::core