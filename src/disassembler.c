#include "disassembler.h"
#include "instr.h"
#include "common.h"
#include <stdio.h>

// Helper function to get register ABI name
const char *reg_abi_name(u32 reg_idx)
{
	const char *reg_abi_names[32] = {
		"zero", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
		"s0",	"s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
		"a6",	"a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
		"s8",	"s9", "s10", "s11", "t3", "t4", "t5", "t6"
	};
	if (reg_idx < 32) {
		return reg_abi_names[reg_idx];
	}
	return "inv";
}

void disassemble(u32 raw_instr, char *buffer, size_t size)
{
	Instruction instr;
	instr_decode(&instr, raw_instr);

	switch (instr.opcode) {
	case 0x33: // R-type
		if (instr.funct7 == 0x01) { // M-extension
			switch (instr.funct3) {
			case 0x0: // MUL
				snprintf(buffer, size, "mul %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x1: // MULH
				snprintf(buffer, size, "mulh %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x2: // MULHSU
				snprintf(buffer, size, "mulhsu %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x3: // MULHU
				snprintf(buffer, size, "mulhu %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x4: // DIV
				snprintf(buffer, size, "div %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x5: // DIVU
				snprintf(buffer, size, "divu %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x6: // REM
				snprintf(buffer, size, "rem %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x7: // REMU
				snprintf(buffer, size, "remu %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			}
		} else {
			switch (instr.funct3) {
			case 0x0: // ADD or SUB
				if (instr.funct7 == 0x00) { // ADD
					snprintf(buffer, size, "add %s, %s, %s",
						 reg_abi_name(instr.rd),
						 reg_abi_name(instr.rs1),
						 reg_abi_name(instr.rs2));
				} else if (instr.funct7 == 0x20) { // SUB
					snprintf(buffer, size, "sub %s, %s, %s",
						 reg_abi_name(instr.rd),
						 reg_abi_name(instr.rs1),
						 reg_abi_name(instr.rs2));
				}
				break;
			case 0x1: // SLL
				snprintf(buffer, size, "sll %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x2: // SLT
				snprintf(buffer, size, "slt %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x3: // SLTU
				snprintf(buffer, size, "sltu %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x4: // XOR
				snprintf(buffer, size, "xor %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x5: // SRL or SRA
				if (instr.funct7 == 0x00) { // SRL
					snprintf(buffer, size, "srl %s, %s, %s",
						 reg_abi_name(instr.rd),
						 reg_abi_name(instr.rs1),
						 reg_abi_name(instr.rs2));
				} else if (instr.funct7 == 0x20) { // SRA
					snprintf(buffer, size, "sra %s, %s, %s",
						 reg_abi_name(instr.rd),
						 reg_abi_name(instr.rs1),
						 reg_abi_name(instr.rs2));
				}
				break;
			case 0x6: // OR
				snprintf(buffer, size, "or %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			case 0x7: // AND
				snprintf(buffer, size, "and %s, %s, %s",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1),
					 reg_abi_name(instr.rs2));
				break;
			}
		}
		break;
	case 0x13: // I-type
		switch (instr.funct3) {
		case 0x0: // ADDI
			snprintf(buffer, size, "addi %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x1: // SLLI
			snprintf(buffer, size, "slli %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x2: // SLTI
			snprintf(buffer, size, "slti %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x3: // SLTIU
			snprintf(buffer, size, "sltiu %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x4: // XORI
			snprintf(buffer, size, "xori %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x5: // SRLI or SRAI
			if (instr.funct7 == 0x00) { // SRLI
				snprintf(buffer, size, "srli %s, %s, %d",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1), instr.imm);
			} else if (instr.funct7 == 0x20) { // SRAI
				snprintf(buffer, size, "srai %s, %s, %d",
					 reg_abi_name(instr.rd),
					 reg_abi_name(instr.rs1), instr.imm);
			}
			break;
		case 0x6: // ORI
			snprintf(buffer, size, "ori %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		case 0x7: // ANDI
			snprintf(buffer, size, "andi %s, %s, %d",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1), instr.imm);
			break;
		}
		break;
	case 0x03: // I-type (loads)
		switch (instr.funct3) {
		case 0x0: // LB
			snprintf(buffer, size, "lb %s, %d(%s)",
				 reg_abi_name(instr.rd), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x1: // LH
			snprintf(buffer, size, "lh %s, %d(%s)",
				 reg_abi_name(instr.rd), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x2: // LW
			snprintf(buffer, size, "lw %s, %d(%s)",
				 reg_abi_name(instr.rd), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x4: // LBU
			snprintf(buffer, size, "lbu %s, %d(%s)",
				 reg_abi_name(instr.rd), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x5: // LHU
			snprintf(buffer, size, "lhu %s, %d(%s)",
				 reg_abi_name(instr.rd), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		}
		break;
	case 0x23: // S-type
		switch (instr.funct3) {
		case 0x0: // SB
			snprintf(buffer, size, "sb %s, %d(%s)",
				 reg_abi_name(instr.rs2), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x1: // SH
			snprintf(buffer, size, "sh %s, %d(%s)",
				 reg_abi_name(instr.rs2), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		case 0x2: // SW
			snprintf(buffer, size, "sw %s, %d(%s)",
				 reg_abi_name(instr.rs2), instr.imm,
				 reg_abi_name(instr.rs1));
			break;
		}
		break;
	case 0x63: // B-type
		switch (instr.funct3) {
		case 0x0: // BEQ
			snprintf(buffer, size, "beq %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		case 0x1: // BNE
			snprintf(buffer, size, "bne %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		case 0x4: // BLT
			snprintf(buffer, size, "blt %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		case 0x5: // BGE
			snprintf(buffer, size, "bge %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		case 0x6: // BLTU
			snprintf(buffer, size, "bltu %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		case 0x7: // BGEU
			snprintf(buffer, size, "bgeu %s, %s, %d",
				 reg_abi_name(instr.rs1),
				 reg_abi_name(instr.rs2), instr.imm);
			break;
		}
		break;
	case 0x37: // LUI
		snprintf(buffer, size, "lui %s, 0x%x", reg_abi_name(instr.rd),
			 (u32)instr.imm >> 12);
		break;
	case 0x17: // AUIPC
		snprintf(buffer, size, "auipc %s, 0x%x", reg_abi_name(instr.rd),
			 (u32)instr.imm >> 12);
		break;
	case 0x6F: // JAL
		snprintf(buffer, size, "jal %s, %d", reg_abi_name(instr.rd),
			 instr.imm);
		break;
	case 0x67: // JALR
		snprintf(buffer, size, "jalr %s, %d(%s)",
			 reg_abi_name(instr.rd), instr.imm,
			 reg_abi_name(instr.rs1));
		break;
	case 0x73: // SYSTEM
		if (instr.imm == 0x0) {
			snprintf(buffer, size, "ecall");
		} else if (instr.imm == 0x1) {
			snprintf(buffer, size, "ebreak");
		}
		break;
	case 0x0F: // FENCE
		snprintf(buffer, size, "fence");
		break;
	case 0x2F: // Atomic
	{
		u32 funct5 = (instr.funct7 & 0b1111100) >> 2;
		switch (funct5) {
		case 0b00010: // LR.W
			snprintf(buffer, size, "lr.w %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs1));
			break;
		case 0b00011: // SC.W
			snprintf(buffer, size, "sc.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b00001: // AMOSWAP.W
			snprintf(buffer, size, "amoswap.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b00000: // AMOADD.W
			snprintf(buffer, size, "amoadd.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b00100: // AMOXOR.W
			snprintf(buffer, size, "amoxor.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b01100: // AMOAND.W
			snprintf(buffer, size, "amoand.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b01000: // AMOOR.W
			snprintf(buffer, size, "amoor.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b10000: // AMOMIN.W
			snprintf(buffer, size, "amomin.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b10100: // AMOMAX.W
			snprintf(buffer, size, "amomax.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b11000: // AMOMINU.W
			snprintf(buffer, size, "amominu.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		case 0b11100: // AMOMAXU.W
			snprintf(buffer, size, "amomaxu.w %s, %s, (%s)",
				 reg_abi_name(instr.rd),
				 reg_abi_name(instr.rs2),
				 reg_abi_name(instr.rs1));
			break;
		default:
			snprintf(buffer, size, "unknown amo");
			break;
		}
	} break;
	default:
		snprintf(buffer, size, "unknown opcode 0x%x", instr.opcode);
		break;
	}
}
