#ifndef RV32I_DISASSEMBLER_H
#define RV32I_DISASSEMBLER_H

#include "type.h"
#include <stddef.h>

void disassemble(u32 raw_instr, char *buffer, size_t size);

#endif /* RV32I_DISASSEMBLER_H */
