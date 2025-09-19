
#ifndef RV32I_MEMORY_H
#define RV32I_MEMORY_H

#include "type.h"

/* Memory allocation */
u8 *memory_create(u32 size);
void memory_destroy(u8 *mem);

/* Memory helpers for little-endian RV32I emulator */

u8 mem_load8(u8 *mem, u32 addr);
u16 mem_load16(u8 *mem, u32 addr);
u32 mem_load32(u8 *mem, u32 addr);

void mem_store8(u8 *mem, u32 addr, u8 val);
void mem_store16(u8 *mem, u32 addr, u16 val);
void mem_store32(u8 *mem, u32 addr, u32 val);

#endif /* RV32I_MEMORY_H */
