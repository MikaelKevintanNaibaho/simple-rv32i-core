#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

u8 *memory_create(u32 size)
{
	u8 *mem = malloc(size);
	if (!mem) {
		fprintf(stderr, "Failed to allocate memory\n");
		exit(1);
	}
	return mem;
}

void memory_destroy(u8 *mem)
{
	free(mem);
}

u8 mem_load8(u8 *mem, u32 addr)
{
	return mem[addr];
}

u16 mem_load16(u8 *mem, u32 addr)
{
	return (u16)(mem[addr]) | ((u16)(mem[addr + 1]) << 8);
}

u32 mem_load32(u8 *mem, u32 addr)
{
	return (u32)(mem[addr]) | ((u32)(mem[addr + 1]) << 8) |
	       ((u32)(mem[addr + 2]) << 16) | ((u32)(mem[addr + 3]) << 24);
}

void mem_store8(u8 *mem, u32 addr, u8 val)
{
	mem[addr] = val;
}

void mem_store16(u8 *mem, u32 addr, u16 val)
{
	mem[addr] = val & 0xFF;
	mem[addr + 1] = (val >> 8) & 0xFF;
}

void mem_store32(u8 *mem, u32 addr, u32 val)
{
	mem[addr] = val & 0xFF;
	mem[addr + 1] = (val >> 8) & 0xFF;
	mem[addr + 2] = (val >> 16) & 0xFF;
	mem[addr + 3] = (val >> 24) & 0xFF;
}
