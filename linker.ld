/* Simple Linker Script for RV32I Emulator */
OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
  /* 64KB of RAM starting at address 0 */
  ram (wx) : ORIGIN = 0x00000000, LENGTH = 64K
}

SECTIONS
{
  .text :
  {
    *(.text.init) /* The _start section */
    *(.text)      /* All other code */
  } > ram

  .data :
  {
    *(.data)      /* All initialized data */
  } > ram
}
