#ifndef RV32I_CPU_H
#define RV32I_CPU_H

#include "type.h"

/* RISC-V RV32I constants */
#define XLEN 32 /* Register width */
#define NREGS 32 /* Number of Integer Register */
#define MEM_SIZE 65536 /* Default memmory size: 64KB */

/* CPU state */

enum cpu_state {
	CPU_STATE_RUNNING,
	CPU_STATE_HALTED,

};

struct cpu {
	u32 registers[NREGS]; // 32 general-purpose registers
	u32 prev_registers[NREGS]; // Store previous register values
	u32 pc; // program counter
	u32 reservation_set;
	u32 reservation_address;
	u8 *memory; // system memory
	enum cpu_state state; // state field
};

/* CPU interface */
struct cpu *cpu_create(u32 mem_size);
void cpu_destroy(struct cpu *c);
void cpu_reset(struct cpu *c);
void cpu_step(struct cpu *c);
void cpu_run(struct cpu *c);

#endif /* RV32I_CPU_H */
