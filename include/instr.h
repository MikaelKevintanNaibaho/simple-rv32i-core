#ifndef RV32I_INSTR_H
#define RV32I_INSTR_H

#include "type.h"
#include "cpu.h"

/* Instruction field extractors (for 32-bit instr) */
#define OPCODE(i) ((i) & 0x7f)
#define RD(i) (((i) >> 7) & 0x1f)
#define FUNCT3(i) (((i) >> 12) & 0x7)
#define RS1(i) (((i) >> 15) & 0x1f)
#define RS2(i) (((i) >> 20) & 0x1f)
#define FUNCT7(i) (((i) >> 25) & 0x7f)

/* Sign-extension helper */
static inline s32 sign_extend(u32 val, int bits)
{
	s32 m = 1U << (bits - 1);
	return (val ^ m) - m;
}

/* Instruction executor */
void instr_exec(struct cpu *c, u32 instr);

#endif /* RV32I_INSTR_H */
