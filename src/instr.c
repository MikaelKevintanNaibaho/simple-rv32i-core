#include "instr.h"
#include "common.h"
#include "cpu.h"
#include "memory.h"
#include <stdio.h>

// Extract bit range [hi:lo] from x
static inline u32 get_bits(u32 x, int hi, int lo)
{
	return (x >> lo) & ((1u << (hi - lo + 1)) - 1);
}

// Decodes a raw 32-bit instruction into an Instruction struct.
void instr_decode(Instruction *instr, u32 raw)
{
	instr->opcode = get_bits(raw, 6, 0);
	instr->rd = get_bits(raw, 11, 7);
	instr->funct3 = get_bits(raw, 14, 12);
	instr->rs1 = get_bits(raw, 19, 15);
	instr->rs2 = get_bits(raw, 24, 20);
	instr->funct7 = get_bits(raw, 31, 25);

	// Default immediate to 0
	instr->imm = 0;

	// Decode the immediate value based on the instruction format (opcode)
	switch (instr->opcode) {
	case 0x13: { // I-type (ADDI, SLLI, etc.)
		// Shift instructions (SLLI, SRLI, SRAI) have a special immediate format
		// that is NOT sign-extended....
		if (instr->funct3 == 0x1 || instr->funct3 == 0x5) {
			instr->imm = (s32)get_bits(raw, 24, 20);
			instr->funct7 = get_bits(raw, 31, 25);
		} else {
			// Other I-type instructions like ADDI have a sign-extended immediate.
			instr->imm = sign_extend(get_bits(raw, 31, 20), 12);
		}
		break;
	}
	case 0x03: // I-type loads
	case 0x67: // JALR
		instr->imm = sign_extend(get_bits(raw, 31, 20), 12);
		break;

	case 0x23: { // S-type (stores)
		u32 imm11_5 = get_bits(raw, 31, 25);
		u32 imm4_0 = get_bits(raw, 11, 7);
		instr->imm = sign_extend((imm11_5 << 5) | imm4_0, 12);
		break;
	}

	case 0x63: { // B-type (branches)
		u32 imm12 = get_bits(raw, 31, 31);
		u32 imm10_5 = get_bits(raw, 30, 25);
		u32 imm4_1 = get_bits(raw, 11, 8);
		u32 imm11 = get_bits(raw, 7, 7);
		instr->imm = sign_extend((imm12 << 12) | (imm11 << 11) |
						 (imm10_5 << 5) | (imm4_1 << 1),
					 13);
		break;
	}

	case 0x37: // LUI
	case 0x17: // AUIPC
		// U-type immediate covers bits 31-12
		instr->imm = (s32)(raw & 0xFFFFF000);
		break;

	case 0x6F: { // J-type (JAL)
		u32 imm20 = get_bits(raw, 31, 31);
		u32 imm10_1 = get_bits(raw, 30, 21);
		u32 imm11 = get_bits(raw, 20, 20);
		u32 imm19_12 = get_bits(raw, 19, 12);
		instr->imm = sign_extend((imm20 << 20) | (imm19_12 << 12) |
						 (imm11 << 11) | (imm10_1 << 1),
					 21);
		break;
	}

	case 0x73:
		instr->imm = sign_extend(get_bits(raw, 31, 20), 12);
		break;

		// R-type instructions do not have an immediate field.
	case 0x33:
	default:
		instr->imm = 0;
	}
}

// --- Instruction Execution Functions ---

static void r_type_exec(struct cpu *c, const Instruction *instr)
{
	u32 rd = instr->rd;
	u32 rs1 = instr->rs1;
	u32 rs2 = instr->rs2;

	switch (instr->funct3) {
	case 0x0: // ADD or SUB
		if (instr->funct7 == 0x00) { // ADD
			c->registers[rd] =
				c->registers[rs1] + c->registers[rs2];
		} else if (instr->funct7 == 0x20) { // SUB
			c->registers[rd] =
				c->registers[rs1] - c->registers[rs2];
		}
		break;
	case 0x1: // SLL
		u32 shamt = c->registers[rs2] & 0x1F;
		c->registers[rd] = c->registers[rs1] << shamt;
		break;
	case 0x2: // SLT
		c->registers[rd] =
			((s32)c->registers[rs1] < (s32)c->registers[rs2]) ? 1 :
									    0;
		break;
	case 0x3: // SLTU (Set if Less Than, Unsigned)
		// Default comparison is unsigned.
		c->registers[rd] = (c->registers[rs1] < c->registers[rs2]) ? 1 :
									     0;
		break;
	case 0x4: // XOR
		c->registers[rd] = c->registers[rs1] ^ c->registers[rs2];
		break;
	case 0x05: { // SRL or SRA
		// use only the lower 5 bits of rs2 as the shift amount
		u32 shamt = c->registers[rs2] & 0x1F;
		if (instr->funct7 == 0x00) { // SRL
			c->registers[rd] = c->registers[rs1] >> shamt;
		} else if (instr->funct7 == 0x20) { // SRA
			c->registers[rd] = (s32)c->registers[rs1] >> shamt;
		}
		break;
	}

	case 0x6: // OR
		c->registers[rd] = c->registers[rs1] | c->registers[rs2];
		break;
	case 0x7: // AND
		c->registers[rd] = c->registers[rs1] & c->registers[rs2];
		break;
	}
}

static void i_type_exec(struct cpu *c, const Instruction *instr)
{
	u32 rd = instr->rd;
	u32 rs1 = instr->rs1;
	s32 imm = instr->imm;

	switch (instr->funct3) {
	case 0x0: // ADDI
		c->registers[rd] = c->registers[rs1] + imm;
		break;

	case 0x1: { // SLLI (Shift Left Logical Immediate)

		u32 shamt = (u32)imm & 0x1F;
		c->registers[rd] = c->registers[rs1] << shamt;
		break;
	}
	case 0x2: // SLTI
		c->registers[rd] = ((s32)c->registers[rs1] < imm) ? 1 : 0;
		break;
	case 0x3: // SLTIU
		c->registers[rd] = (c->registers[rs1] < (u32)imm) ? 1 : 0;
		break;
	case 0x4: // XORI
		c->registers[rd] = c->registers[rs1] ^ imm;
		break;

	case 0x5: { // SRLI or SRAI
		u32 shamt = instr->imm & 0x1F;
		if (instr->funct7 == 0x00) { // SRLI
			c->registers[rd] = ((u32)c->registers[rs1]) >> shamt;
		} else if (instr->funct7 == 0x20) { // SRAI
			// For arithmetic right shift, cast to signed int32
			c->registers[rd] = ((s32)c->registers[rs1]) >> shamt;
		}
		break;
	}
	case 0x6: // ORI
		c->registers[rd] = c->registers[rs1] | imm;
		break;
	case 0x7: // ANDI
		c->registers[rd] = c->registers[rs1] & imm;
		break;
	}
}

static void b_type_exec(struct cpu *c, const Instruction *instr)
{
	bool take_branch = false;
	u32 val1 = c->registers[instr->rs1];
	u32 val2 = c->registers[instr->rs2];

	switch (instr->funct3) {
	case 0x0: // BEQ
		take_branch = (val1 == val2);
		break;
	case 0x1: // BNE
		take_branch = (val1 != val2);
		break;
	case 0x4: // BLT (signed)
		take_branch = ((s32)val1 < (s32)val2);
		break;
	case 0x5: // BGE (signed)
		take_branch = ((s32)val1 >= (s32)val2);
		break;
	case 0x6: // BLTU (unsigned)
		take_branch = (val1 < val2);
		break;
	case 0x7: // BGEU (unsigned)
		take_branch = (val1 >= val2);
		break;
	}

	if (take_branch) {
		// The PC will be incremented by 4 in cpu_step, so we pre-compensate.
		c->pc += instr->imm - 4;
	}
}

static void u_type_exec(struct cpu *c, const Instruction *instr)
{
	switch (instr->opcode) {
	case 0x37: // LUI
		c->registers[instr->rd] = instr->imm;
		break;
	case 0x17: // AUIPC
		c->registers[instr->rd] = c->pc + instr->imm;
		break;
	}
}

static void j_type_exec(struct cpu *c, const Instruction *instr)
{
	// Store the return address (pc + 4) if rd is not the zero register.
	if (instr->rd != 0) {
		c->registers[instr->rd] = c->pc + 4;
	}
	// Jump to the new address, compensating for the upcoming pc += 4.
	c->pc += instr->imm - 4;
}

static void store_exec(struct cpu *c, const Instruction *instr)
{
	// Calculate the memory address: base register (rs1) + immediate offset
	u32 addr = c->registers[instr->rs1] + instr->imm;

	// get the value to store from the source register (r2)
	u32 val = c->registers[instr->rs2];

	switch (instr->funct3) {
	case 0x00: // SB
		mem_store8(c->memory, addr, (u8)val);
		break;
	case 0x01: // SH
		mem_store16(c->memory, addr, (u16)val);
		break;
	case 0x02: // SW
		mem_store32(c->memory, addr, val);
	}
}

static void load_exec(struct cpu *c, const Instruction *instr)
{
	u32 addr = c->registers[instr->rs1] + instr->imm;
	u32 rd = instr->rd;

	switch (instr->funct3) {
	case 0x0: // LB (Load Byte, sign-extended)
		c->registers[rd] = (s32)(s8)mem_load8(c->memory, addr);
		break;
	case 0x1: // LH (Load Half-word, sign-extended)
		c->registers[rd] = (s32)(s16)mem_load16(c->memory, addr);
		break;
	case 0x2: // LW (Load Word)
		c->registers[rd] = mem_load32(c->memory, addr);
		break;
	case 0x4: // LBU (Load Byte, zero-extended)
		c->registers[rd] = mem_load8(c->memory, addr);
		break;
	case 0x5: // LHU (Load Half-word, zero-extended)
		c->registers[rd] = mem_load16(c->memory, addr);
		break;
	}
}

static void jalr_exec(struct cpu *c, const Instruction *instr)
{
	// Store the return address before changing the PC.
	u32 return_addr = c->pc + 4;

	// Calculate the target address: (rs1 + imm) and clear the LSB.
	u32 target_addr = (c->registers[instr->rs1] + instr->imm) & ~1U;

	// Jump, compensating for the upcoming pc += 4.
	c->pc = target_addr - 4;

	// Write the return address to rd if it's not x0.
	if (instr->rd != 0) {
		c->registers[instr->rd] = return_addr;
	}
}

static void system_exec(struct cpu *c, const Instruction *instr)
{
	switch (instr->imm) {
	case 0x0: // ECALL
		printf("ECALL executed. Halting,\n");
		c->state = CPU_STATE_HALTED;
		/* to halt correctly, the PC should point to this instruction.
        * substract 4 to counteract the 'pc += 4' in cpu_step().*/
		c->pc -= 4;
		break;
	case 0x1: // EBREAK
		printf("EBREAK executed. Halting.\n");
		c->state = CPU_STATE_HALTED;
		c->pc -= 4;
		break;
	default:
		printf("Error: Unknown SYSTEM instruction with imm=0x%x\n",
		       instr->imm);
	}
}

// Main execution function: Decodes and then executes an instruction.
void instr_exec(struct cpu *c, u32 raw_instr)
{
	// 1. Decode the raw instruction into a struct.
	Instruction instr;
	instr_decode(&instr, raw_instr);

	// 2. Execute based on the decoded opcode.
	switch (instr.opcode) {
	case 0x33: // R-type (ADD, SUB, etc.)
		r_type_exec(c, &instr);
		break;
	case 0x13: // I-type (ADDI, SLTI, etc.)
		i_type_exec(c, &instr);
		break;
	case 0x37: // LUI
	case 0x17: // AUIPC
		u_type_exec(c, &instr);
		break;
	case 0x6F: // JAL
		j_type_exec(c, &instr);
		break;
	case 0x67: // JALR
		jalr_exec(c, &instr);
		break;
	case 0x63: // B-type (BEQ, BNE, etc.)
		b_type_exec(c, &instr);
		break;
	case 0x03: // Load instructions (LB, LW, etc.)
		load_exec(c, &instr);
		break;
	case 0x23: // Store instructions (SB, SH, SW)
		store_exec(c, &instr);
		break;
	case 0x0F: // MISC-MEM (FENCE, FENCE.I)
		/*currently the emulator has a simple memory model, so FENCE is a no-operation
        * we decode it but do nothing. the PC will advance normally
        * */
		break;
	case 0x73: // SYSTEM instruction (ECALL, EBREAK)
		system_exec(c, &instr);
		break;
	default:
		printf("Error: Unknown opcode=0x%x\n", instr.opcode);
	}

	// The zero register x0 is hardwired to 0 and cannot be written to.
	c->registers[0] = 0;
}
