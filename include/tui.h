#ifndef RV32I_TUI_H
#define RV32I_TUI_H

#include "cpu.h"

// Initializes the ncurses screen and windows
void tui_init();

// Cleans up and closes the ncurses screen
void tui_destroy();

// Updates the TUI with the current CPU state
void tui_update(struct cpu *cpu);

#endif // RV32I_TUI_H
