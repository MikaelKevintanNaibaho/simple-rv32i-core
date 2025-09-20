#include <gtest/gtest.h>

extern "C" {
#include "cpu.h"
#include "memory.h"
}

class RV32ITest : public ::testing::Test {
    protected:
	struct cpu *cpu;

	void SetUp() override
	{
		cpu = cpu_create(MEM_SIZE);
	}

	void TearDown() override
	{
		cpu_destroy(cpu);
	}

	void load_program(const std::vector<uint32_t> &program)
	{
		for (size_t i = 0; i < program.size(); ++i) {
			mem_store32(cpu->memory, i * 4, program[i]);
		}
	}

	void run_program(int instructions)
	{
		for (int i = 0; i < instructions; ++i) {
			cpu_step(cpu);
		}
	}
};

TEST_F(RV32ITest, ADDI)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5
		0x00A00113, // addi x2, x0, 10
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[1], 5);
	EXPECT_EQ(cpu->registers[2], 10);
}

TEST_F(RV32ITest, ADD)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5
		0x00A00113, // addi x2, x0, 10
		0x002081B3, // add x3, x1, x2
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 15);
}

TEST_F(RV32ITest, SUB)
{
	std::vector<uint32_t> program = {
		0x00A00093, // addi x1, x0, 10
		0x00500113, // addi x2, x0, 5
		0x402081B3, // sub x3, x1, x2
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 5);
}

TEST_F(RV32ITest, XOR)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5   (0101)
		0x00300113, // addi x2, x0, 3   (0011)
		0x0020C1B3, // xor x3, x1, x2  (0110 -> 6)
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 6);
}

TEST_F(RV32ITest, OR)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5   (0101)
		0x00300113, // addi x2, x0, 3   (0011)
		0x0020E1B3, // or  x3, x1, x2  (0111 -> 7)
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 7);
}

TEST_F(RV32ITest, AND)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5   (0101)
		0x00300113, // addi x2, x0, 3   (0011)
		0x0020F1B3, // and x3, x1, x2  (0001 -> 1)
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 1);
}

TEST_F(RV32ITest, LUI)
{
	std::vector<uint32_t> program = {
		0x000FF0B7, // lui x1, 0xFF
	};
	load_program(program);
	run_program(program.size());

	// The immediate 0xFF is loaded into the upper 20 bits.
	// 0xFF << 12 = 0xFF000
	EXPECT_EQ(cpu->registers[1], 0xFF000);
}

TEST_F(RV32ITest, AUIPC)
{
	// Set PC to a known address before starting
	cpu->pc = 0x1000;
	uint32_t instruction = 0x00001097; // auipc x1, 1

	// Manually load the single instruction at the PC's address
	mem_store32(cpu->memory, cpu->pc, instruction);

	// We only run one instruction, so pc is 0x1000 during execution
	cpu_step(cpu);

	// Result should be pc + (imm << 12)
	// 0x1000 + (1 << 12) = 0x1000 + 0x1000 = 0x2000
	EXPECT_EQ(cpu->registers[1], 0x2000);
}

TEST_F(RV32ITest, JAL)
{
	std::vector<uint32_t> program = {
		0x008000EF, // 0x00: jal x1, 8
		0x00000013, // 0x04: nop (should be skipped)
		0x00500113, // 0x08: addi x2, x0, 5
	};
	load_program(program);

	// 1. Execute the JAL instruction at pc=0
	cpu_step(cpu);

	// Check the link register (x1). It should hold the return address (0 + 4).
	EXPECT_EQ(cpu->registers[1], 4);

	// Check the program counter. It should have jumped to 0 + 8.
	EXPECT_EQ(cpu->pc, 8);

	// 2. Execute the instruction at the new PC (0x8)
	cpu_step(cpu);

	// Check that the instruction at the destination was executed correctly.
	EXPECT_EQ(cpu->registers[2], 5);
}

TEST_F(RV32ITest, JALR)
{
	std::vector<uint32_t> program = {
		0x008000EF, // 0x00: jal x1, 8         (Call function at 0x08)
		0x00000013, // 0x04: nop                 (This is our return point)
		0x07B00113, // 0x08: addi x2, x0, 123    (The "function" body)
		0x00008067, // 0x0C: jalr x0, x1, 0      (Return using address in x1)
	};
	load_program(program);

	// 1. "Call" the function with JAL
	cpu_step(cpu);
	ASSERT_EQ(cpu->pc, 8); // PC should be at the function start
	ASSERT_EQ(cpu->registers[1], 4); // x1 should hold the return address

	// 2. Execute the function body
	cpu_step(cpu);
	ASSERT_EQ(cpu->registers[2], 123); // x2 should have the result
	ASSERT_EQ(cpu->pc, 12); // PC moved to the jalr instruction

	// 3. "Return" from the function with JALR
	cpu_step(cpu);
	ASSERT_EQ(cpu->pc, 4); // PC should have returned to address 4
}

TEST_F(RV32ITest, BEQ_BranchTaken)
{
	std::vector<uint32_t> program = {
		0x00A00093, // 0x00: addi x1, x0, 10
		0x00A00113, // 0x04: addi x2, x0, 10
		0x00208463, // 0x08: beq x1, x2, 8 (jumps to 0x10)
		0x00100193, // 0x0C: addi x3, x0, 1 (should be skipped)
		0x00200213, // 0x10: addi x4, x0, 2
	};
	load_program(program);

	// Run the first three instructions (addi, addi, beq)
	run_program(3);

	EXPECT_EQ(cpu->registers[3], 0); // x3 should not have been updated
	ASSERT_EQ(cpu->pc, 0x10); // PC should have jumped

	// Run the final instruction
	cpu_step(cpu);
	EXPECT_EQ(cpu->registers[4], 2); // x4 should be updated
}

TEST_F(RV32ITest, BEQ_BranchNotTaken)
{
	std::vector<uint32_t> program = {
		0x00A00093, // 0x00: addi x1, x0, 10
		0x06300113, // 0x04: addi x2, x0, 99
		0x00208463, // 0x08: beq x1, x2, 8
		0x00100193, // 0x0C: addi x3, x0, 1 (should be executed)
	};
	load_program(program);

	// Run the first three instructions (addi, addi, beq)
	run_program(3);

	ASSERT_EQ(cpu->pc, 0x0C); // PC should be at the next instruction

	// Run the next instruction
	cpu_step(cpu);
	EXPECT_EQ(cpu->registers[3], 1); // x3 should be updated
}

TEST_F(RV32ITest, Loads)
{
	// Set up a base address in register x1
	uint32_t base_addr = 0x100;
	cpu->registers[1] = base_addr;

	// Store a known 32-bit value in memory. 0x80ABCDEF has components
	// that are negative when interpreted as 8 or 16-bit signed numbers.
	uint32_t test_val = 0x80ABCDEF;
	mem_store32(cpu->memory, base_addr, test_val);

	// Instructions to test all 5 load types from base_addr (x1) with offset 0
	std::vector<uint32_t> program = {
		0x0000A103, // lw  x2, 0(x1)
		0x00008183, // lb  x3, 0(x1)
		0x0000C203, // lbu x4, 0(x1)
		0x00009283, // lh  x5, 0(x1)
		0x0000D303, // lhu x6, 0(x1)
	};
	load_program(program);
	run_program(program.size());

	// 1. LW: Load the full word
	EXPECT_EQ(cpu->registers[2], 0x80ABCDEF);

	// 2. LB: Load byte 0xEF (-17). Sign-extended should be 0xFFFFFFEF.
	EXPECT_EQ(cpu->registers[3], 0xFFFFFFEF);

	// 3. LBU: Load byte 0xEF. Zero-extended should be 0x000000EF.
	EXPECT_EQ(cpu->registers[4], 0x000000EF);

	// 4. LH: Load half-word 0xCDEF. Sign-extended should be 0xFFFFCDEF.
	EXPECT_EQ(cpu->registers[5], 0xFFFFCDEF);

	// 5. LHU: Load half-word 0xCDEF. Zero-extended should be 0x0000CDEF.
	EXPECT_EQ(cpu->registers[6], 0x0000CDEF);
}

TEST_F(RV32ITest, Stores)
{
	// Set up base address in x1 and data in x2
	uint32_t base_addr = 0x200;
	cpu->registers[1] = base_addr;
	cpu->registers[2] = 0xDEADBEEF; // The value we want to store

	// Instructions to test all 3 store types from base_addr (x1)
	std::vector<uint32_t> program = {
		// Store the full word 0xDEADBEEF at address 0x200
		0x0020A023, // sw x2, 0(x1)

		// Store the lower half-word 0xBEEF at address 0x204
		0x00209223, // sh x2, 4(x1)

		// Store the lowest byte 0xEF at address 0x206
		0x00208323, // sb x2, 6(x1)
	};
	load_program(program);
	run_program(program.size());

	// 1. SW: Verify the full word was stored correctly
	EXPECT_EQ(mem_load32(cpu->memory, base_addr + 0), 0xDEADBEEF);

	// 2. SH: Verify the half-word was stored correctly (memory is little-endian)
	EXPECT_EQ(mem_load16(cpu->memory, base_addr + 4), 0xBEEF);

	// 3. SB: Verify the byte was stored correctly
	EXPECT_EQ(mem_load8(cpu->memory, base_addr + 6), 0xEF);
}

TEST_F(RV32ITest, Shifts)
{
	cpu->registers[1] = 0x0800000F;

	std::vector<uint32_t> program = {
		// x2 = x1 << 4  (0x0800000F << 4 = 0x800000F0) - no overflow!
		0x00409113, // slli x2, x1, 4
		// x3 = x1 >> 4  (logical: 0x0800000F >> 4 = 0x00800000)
		0x0040d193, // srli x3, x1, 4
		// x4 = x1 >> 4  (arithmetic: 0x0800000F >> 4 = 0x00800000, positive number)
		0x4040d213, // srai x4, x1, 4
	};

	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[2], 0x800000F0); // Now achievable!
	EXPECT_EQ(cpu->registers[3], 0x00800000); // Updated expectation
	EXPECT_EQ(cpu->registers[4],
		  0x00800000); // Same as SRLI for positive numbers
}

TEST_F(RV32ITest, RTypeShiftsAndCompares)
{
	// Setup initial values
	cpu->registers[1] = 0x0800000F; //large  positive number
	cpu->registers[2] = 4; // Shift amount
	cpu->registers[3] = 0x8000000F; // A small positive number

	std::vector<uint32_t> program = {
		// x4 = x1 << x2  (0x0800000F << 4 = 0x800000F0)
		0x00209233, // sll x4, x1, x2

		// x5 = x1 < x3 (signed:  0x0800000F < 0x8000000F is false)
		0x0030a2b3, // slt x5, x1, x3

		// x6 = x1 < x3 (unsigned: 0x0800000F < 0x8000000F is true)
		0x0030b333, // sltu x6, x1, x3
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[4], 0x800000F0); // SLL result
	EXPECT_EQ(cpu->registers[5], 0); // SLT result
	EXPECT_EQ(cpu->registers[6], 1); // SLTU result
}

TEST_F(RV32ITest, RTypeRightShifts)
{
	// Setup initial values
	cpu->registers[1] = 0x8000000F; // A negative number
	cpu->registers[2] = 4; // Shift amount

	std::vector<uint32_t> program = {
		// x3 = x1 >> x2 (logical: shifts in zeros -> 0x08000000)
		0x0020d1b3, // srl x3, x1, x2

		// x4 = x1 >> x2 (arithmetic: shifts in sign bit -> 0xF8000000)
		0x4020d233, // sra x4, x1, x2
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 0x08000000); // SRL result
	EXPECT_EQ(cpu->registers[4], 0xF8000000); // SRA result
}

TEST_F(RV32ITest, SystemInstructions)
{
	// 1. Test FENCE (should be a NOP)
	cpu->pc = 0;
	std::vector<uint32_t> program_fence = {
		0x0000000F, // fence
	};
	load_program(program_fence);
	cpu_step(cpu);
	EXPECT_EQ(cpu->pc, 4); // PC should just advance
	EXPECT_EQ(cpu->state, CPU_STATE_RUNNING); // State should not change

	// 2. Test ECALL (should halt)
	cpu_reset(cpu); // Reset CPU for the next test
	cpu->pc = 0;
	std::vector<uint32_t> program_ecall = {
		0x00000073, // ecall
	};
	load_program(program_ecall);
	cpu_step(cpu);
	EXPECT_EQ(cpu->pc, 0); // PC should point to the ecall instruction
	EXPECT_EQ(cpu->state, CPU_STATE_HALTED); // State should be HALTED

	// 3. Test EBREAK (should halt)
	cpu_reset(cpu); // Reset CPU
	cpu->pc = 0;
	std::vector<uint32_t> program_ebreak = {
		0x00100073, // ebreak
	};
	load_program(program_ebreak);
	cpu_step(cpu);
	EXPECT_EQ(cpu->pc, 0); // PC should point to the ebreak instruction
	EXPECT_EQ(cpu->state, CPU_STATE_HALTED);
}

TEST_F(RV32ITest, MUL)
{
	std::vector<uint32_t> program = {
		0x00500093, // addi x1, x0, 5
		0x00A00113, // addi x2, x0, 10
		0x022081B3, // mul x3, x1, x2 (5 * 10 = 50)
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 50);
}

TEST_F(RV32ITest, DIV)
{
	std::vector<uint32_t> program = {
		0x06400093, // addi x1, x0, 100
		0x00A00113, // addi x2, x0, 10
		0x0220C1B3, // div x3, x1, x2 (100 / 10 = 10)
	};
	load_program(program);
	run_program(program.size());

	EXPECT_EQ(cpu->registers[3], 10);
}
