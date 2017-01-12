typedef void (arm::*thumb_instruction)(u16);
static const thumb_instruction thumb_lut[1024];

template <int imm, int type>
void thumb_1(u16 instruction);

template <bool immediate, bool subtract, int field3>
void thumb_2(u16 instruction);

template <int op, int reg_dest>
void thumb_3(u16 instruction);

template <int op>
void thumb_4(u16 instruction);

template <int op, bool high1, bool high2>
void thumb_5(u16 instruction);

template <int reg_dest>
void thumb_6(u16 instruction);

template <int op, int reg_offset>
void thumb_7(u16 instruction);

template <int op, int reg_offset>
void thumb_8(u16 instruction);

template <int op, int imm>
void thumb_9(u16 instruction);

template <bool load, int imm>
void thumb_10(u16 instruction);

template <bool load, int reg_dest>
void thumb_11(u16 instruction);

template <bool stackptr, int reg_dest>
void thumb_12(u16 instruction);

template <bool sub>
void thumb_13(u16 instruction);

template <bool pop, bool rbit>
void thumb_14(u16 instruction);

template <bool load, int reg_base>
void thumb_15(u16 instruction);

template <int cond>
void thumb_16(u16 instruction);

void thumb_17(u16 instruction);
void thumb_18(u16 instruction);

template <bool h>
void thumb_19(u16 instruction);
