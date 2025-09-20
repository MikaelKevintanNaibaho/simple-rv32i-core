#include "cpu.h"
#include "memory.h"
#include "tui.h"
#include <stdio.h>
#include <stdlib.h> // Required for exit()
#include <ncurses.h>

// ANSI color codes
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

// A more compact and readable register print function
void print_registers(struct cpu *cpu)
{
	printf("PC: 0x%08x\n", cpu->pc);
	printf("-----------------------------------------------------\n");
	for (int i = 0; i < 32; i++) {
		if (cpu->registers[i] != cpu->prev_registers[i]) {
			// Print changed registers in green
			printf(ANSI_COLOR_GREEN
			       "x%-2d: 0x%08x   " ANSI_COLOR_RESET,
			       i, cpu->registers[i]);
		} else {
			// Print unchanged registers in the default color
			printf("x%-2d: 0x%08x   ", i, cpu->registers[i]);
		}

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
	size_t bytes_read = fread(cpu->memory, 1, MEM_SIZE, fp);
	fclose(fp);

	// --- Initialize TUI ---
	tui_init();

	// --- Main Execution Loop ---
	int ch;
	tui_update(cpu); // Initial draw

	while ((ch = getch()) != 'q') {
		if (ch == 's') {
			if (cpu->state == CPU_STATE_RUNNING) {
				cpu_step(cpu);
			}
			tui_update(cpu);
		}
	}

	// --- Cleanup ---
	tui_destroy();
	cpu_destroy(cpu);

	printf("Emulator exited gracefully after loading %zu bytes.\n",
	       bytes_read);
	return 0;
}
