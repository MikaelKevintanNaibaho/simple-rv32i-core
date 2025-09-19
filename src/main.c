#include "cpu.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h> // Required for exit()

// A more compact and readable register print function
void print_registers(struct cpu *cpu)
{
	printf("PC: 0x%08x\n", cpu->pc);
	printf("-----------------------------------------------------\n");
	for (int i = 0; i < 32; i++) {
		// Print register name (e.g., "x10") and its hex value
		printf("x%-2d: 0x%08x   ", i, cpu->registers[i]);

		// Print a newline after every 4th register
		if ((i + 1) % 4 == 0) {
			printf("\n");
		}
	}
	printf("-----------------------------------------------------\n");
}

int main(void)
{
	// Create CPU with 64KB memory
	struct cpu *cpu = cpu_create(MEM_SIZE);

	// --- Load program from file ---
	const char *filename = "program.bin";
	FILE *fp = fopen(filename, "rb"); // Open in binary read mode
	if (fp == NULL) {
		fprintf(stderr, "Error: Cannot open file '%s'.\n", filename);
		fprintf(stderr,
			"Please create a 'program.bin' file using a RISC-V assembler.\n");
		cpu_destroy(cpu);
		exit(1);
	}

	// Get the size of the file to know how many bytes to read
	fseek(fp, 0, SEEK_END);
	long program_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (program_size > MEM_SIZE) {
		fprintf(stderr,
			"Error: Program size (%ld bytes) is larger than memory size (%d bytes).\n",
			program_size, MEM_SIZE);
		fclose(fp);
		cpu_destroy(cpu);
		exit(1);
	}

	// Read the entire file into the beginning of the CPU's memory
	size_t bytes_read = fread(cpu->memory, 1, program_size, fp);
	if (bytes_read != (size_t)program_size) {
		fprintf(stderr, "Error while reading file '%s'.\n", filename);
		fclose(fp);
		cpu_destroy(cpu);
		exit(1);
	}
	fclose(fp);

	printf("Successfully loaded %zu bytes from '%s'.\n", bytes_read,
	       filename);

	// Loop runs until the CPU state becomes HALTED.
	printf("\n--- Starting Execution ---\n");
	int step = 0;
	while (cpu->state == CPU_STATE_RUNNING) {
		printf("\nExecuting step %d at PC=0x%x\n", step++, cpu->pc);

		// Fetch and print the raw instruction for debugging
		u32 raw_instr = mem_load32(cpu->memory, cpu->pc);
		printf("Raw Instruction: 0x%08x\n", raw_instr);

		cpu_step(cpu);
		print_registers(cpu);
	}

	printf("\n--- Program finished ---\n");
	cpu_destroy(cpu);
	return 0;
}
