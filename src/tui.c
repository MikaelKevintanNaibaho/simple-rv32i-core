#include "tui.h"
#include "memory.h"
#include <ncurses.h>
#include <string.h>

// Define window pointers
WINDOW *reg_win;
WINDOW *cpu_win;
WINDOW *mem_win;

// Standard RISC-V ABI register names for better readability
const char *reg_abi_names[32] = { "zero", "ra", "sp", "gp", "tp", "t0",	 "t1",
				  "t2",	  "s0", "s1", "a0", "a1", "a2",	 "a3",
				  "a4",	  "a5", "a6", "a7", "s2", "s3",	 "s4",
				  "s5",	  "s6", "s7", "s8", "s9", "s10", "s11",
				  "t3",	  "t4", "t5", "t6" };

void tui_init()
{
	// Start ncurses mode
	initscr();
	// Disable line buffering
	cbreak();
	// Don't echo while we do getch
	noecho();
	// Enable color
	start_color();
	// Hide the cursor
	curs_set(0);

	// Define color pair 1 (green for changed registers)
	init_pair(1, COLOR_GREEN, COLOR_BLACK);

	// Get screen dimensions
	int height, width;
	getmaxyx(stdscr, height, width);

	// Create windows
	reg_win = newwin(18, 56, 1, 1);
	cpu_win = newwin(5, width - 59, 1, 58);
	mem_win = newwin(height - 7, width - 59, 7, 58);

	// Enable keypad for the main window
	keypad(stdscr, TRUE);

	// Initial draw
	refresh();
}

void tui_destroy()
{
	// Clean up and end ncurses mode
	delwin(reg_win);
	delwin(cpu_win);
	delwin(mem_win);
	endwin();
}

void draw_borders(WINDOW *win, const char *title)
{
	box(win, 0, 0);
	mvwprintw(win, 0, 2, " %s ", title);
}

void tui_update(struct cpu *cpu)
{
	werase(reg_win);
	werase(cpu_win);
	werase(mem_win);

	// --- Draw Register Window (New 2-Column Layout) ---
	draw_borders(reg_win, "Registers");
	for (int i = 0; i < 16; i++) {
		// --- Left Column ---
		int reg_idx1 = i;
		attr_t attr1 = (cpu->registers[reg_idx1] !=
				cpu->prev_registers[reg_idx1]) ?
				       COLOR_PAIR(1) :
				       A_NORMAL;

		wattron(reg_win, attr1);
		mvwprintw(reg_win, i + 1, 2, "x%-2d (%-4s): 0x%08x", reg_idx1,
			  reg_abi_names[reg_idx1], cpu->registers[reg_idx1]);
		wattroff(reg_win, attr1);

		// --- Right Column ---
		int reg_idx2 = i + 16;
		attr_t attr2 = (cpu->registers[reg_idx2] !=
				cpu->prev_registers[reg_idx2]) ?
				       COLOR_PAIR(1) :
				       A_NORMAL;

		wattron(reg_win, attr2);
		mvwprintw(reg_win, i + 1, 29, "x%-2d (%-4s): 0x%08x", reg_idx2,
			  reg_abi_names[reg_idx2], cpu->registers[reg_idx2]);
		wattroff(reg_win, attr2);
	}

	// --- Draw CPU Status Window ---
	draw_borders(cpu_win, "CPU Status");
	u32 instruction = mem_load32(cpu->memory, cpu->pc);
	mvwprintw(cpu_win, 1, 2, "PC          : 0x%08x", cpu->pc);
	mvwprintw(cpu_win, 2, 2, "Instruction : 0x%08x", instruction);
	mvwprintw(cpu_win, 3, 2, "State       : %s",
		  (cpu->state == CPU_STATE_RUNNING) ? "Running" : "Halted");

	// --- Draw Memory Window ---
	draw_borders(mem_win, "Memory View (from PC)");
	for (int i = 0; i < 20; i++) { // Show a bit more memory
		u32 addr = cpu->pc + (i * 4);
		if (addr < MEM_SIZE - 4) {
			u32 value = mem_load32(cpu->memory, addr);
			if (addr == cpu->pc) {
				wattron(mem_win, A_REVERSE);
				mvwprintw(mem_win, i + 1, 2, "> 0x%08x: 0x%08x",
					  addr, value);
				wattroff(mem_win, A_REVERSE);
			} else {
				mvwprintw(mem_win, i + 1, 2, "  0x%08x: 0x%08x",
					  addr, value);
			}
		}
	}

	wrefresh(reg_win);
	wrefresh(cpu_win);
	wrefresh(mem_win);

	mvprintw(20, 2, "Press 's' to step, 'q' to quit.");
	refresh();
}
