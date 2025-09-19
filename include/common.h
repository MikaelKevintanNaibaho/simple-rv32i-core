#ifndef RV32I_COMMON_H
#define RV32I_COMMON_H

#include "type.h"

// Instruction breakdown
typedef struct {
	u32 opcode; // Operation code
	u32 rd; // Destination register
	u32 funct3; // 3-bit function code
	u32 rs1; // Source register 1
	u32 rs2; // Source register 2
	u32 funct7; // 7-bit function code
	s32 imm; // immediate value
} Instruction;

// Memory
typedef struct {
	u8 *mem; // pointer to memory block
	u32 size; // size of the memory in bytes
} Memory;

#endif /* RV32I_COMMON_H */
