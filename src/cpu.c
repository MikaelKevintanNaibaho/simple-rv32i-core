#include "cpu.h"
#include "memory.h"
#include "instr.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cpu *cpu_create(u32 mem_size)
{
	struct cpu *c = malloc(sizeof(struct cpu));
	if (!c) {
		fprintf(stderr, "Failed to allocate CPU\n");
		exit(1);
	}

	c->pc = 0;
	memset(c->registers, 0, sizeof(c->registers));
	memset(c->prev_registers, 0, sizeof(c->prev_registers));
	c->memory = memory_create(mem_size);
	c->state = CPU_STATE_RUNNING;
	c->reservation_set = 0;
	c->reservation_address = 0;
	memset(c->output_buffer, 0, OUTPUT_BUFFER_SIZE);
	c->output_buffer_pos = 0;

	return c;
}

void cpu_destroy(struct cpu *c)
{
	if (!c)
		return;
	memory_destroy(c->memory);
	free(c);
}

void cpu_reset(struct cpu *c)
{
	if (!c)
		return;
	c->pc = 0;
	memset(c->registers, 0, sizeof(c->registers));
	memset(c->prev_registers, 0, sizeof(c->prev_registers));
	memset(c->memory, 0, MEM_SIZE);
	c->state = CPU_STATE_RUNNING;
	c->reservation_set = 0;
	c->reservation_address = 0;
	memset(c->output_buffer, 0, OUTPUT_BUFFER_SIZE);
	c->output_buffer_pos = 0;
}

void cpu_step(struct cpu *c)
{
	// Store current registers before execution
	memcpy(c->prev_registers, c->registers, sizeof(c->registers));

	// Fetch the raw instruction
	u32 raw_instr = mem_load32(c->memory, c->pc);

	// Execute the instruction
	instr_exec(c, raw_instr);

	// Move to the next instruction
	c->pc += 4;
}

void cpu_run(struct cpu *c)
{
	while (c->state == CPU_STATE_RUNNING) {
		cpu_step(c);
	}
}
