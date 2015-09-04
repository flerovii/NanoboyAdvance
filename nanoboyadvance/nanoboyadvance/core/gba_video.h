/*
* Copyright (C) 2015 Frederic Meyer
*
* This file is part of nanoboyadvance.
*
* nanoboyadvance is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* nanoboyadvance is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with nanoboyadvance. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <cstring>
#include "common/types.h"
#include "common/log.h"
#include "gba_io.h"

using namespace std;

namespace NanoboyAdvance
{
    class GBAVideo
    {
        enum class GBAVideoState
        {
            Scanline,
            HBlank,
            VBlank
        };
        GBAIO* gba_io;
        GBAVideoState state;
        int ticks;
		inline uint DecodeRGB5(ushort color);
		inline uint* DecodeTileLine4BPP(uint block_base, uint palette_base, int number, int line);
		inline uint* DecodeTileLine8PP(uint block_base, int number, int line, bool sprite);
		void Render(int line);
    public:
        bool render_scanline;
        ubyte pal[0x400];
        ubyte vram[0x18000];
        ubyte obj[0x400];
		uint buffer[240 * 160];
        GBAVideo(GBAIO* gba_io);
        void Step();
    };
}