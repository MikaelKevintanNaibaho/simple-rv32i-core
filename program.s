# program.s
# A comprehensive test program for the RV32I emulator.
# It covers all implemented instruction types.

.globl _start

# --- TEXT (CODE) SECTION ---
.text
_start:
  # --- Setup ---
  addi  x5, x0, 10
  addi  x6, x0, 5
  addi  x28, x0, -10
  addi  x29, x0, -5

  # --- I-Type: Immediate Arithmetic ---
  addi  x1, x0, 100
  slti  x2, x1, 200
  sltiu x3, x1, 50
  xori  x1, x1, 0x1F0
  ori   x1, x1, 0x111
  andi  x4, x1, 0x0F0

  # --- R-Type: Register-Register Arithmetic ---
  add   x7, x5, x6
  sub   x8, x5, x6
  xor   x9, x5, x6
  or    x11, x5, x6
  and   x12, x5, x6

  # --- R-Type: More Register-Register ---
  li    x1, 0x0800000F   # Load a negative number into x1
  li    x2, 4            # Load shift amount into x2
  li    x3, 1            # Load a positive number into x3
  sll   x4, x1, x2       # x4 = x1 << x2
  slt   x10, x1, x3      # x10 = 1 (signed comparison)
  sltu  x11, x1, x3      # x11 = 0 (unsigned comparison)

  # --- I-Type: Shift Instructions ---
  lui   x20, 0x08000      # Load 0x80000 into the upper 20 bits of x20
  addi  x20, x20, 15      # Add 15 (0xF) to the value in x20
  slli  x21, x20, 4         # x21 = x20 << 4  (logical left shift)
  srli  x22, x20, 4         # x22 = x20 >> 4  (logical right shift)
  srai  x23, x20, 4         # x23 = x20 >> 4  (arithmetic right shift)

  # --- U-Type: Upper Immediate Instructions ---
  lui   x13, 0xABCDE
  auipc x14, 0x1

  # --- Memory: Load Instructions ---
  la    x10, test_data
  lw    x15, 0(x10)
  lh    x16, 2(x10)
  lhu   x17, 2(x10)
  lb    x18, 3(x10)
  lbu   x19, 3(x10)

  # --- Memory: Store Instructions ---
  la    x10, store_target
  sw    x15, 0(x10)
  sh    x15, 4(x10)
  sb    x15, 6(x10)

  # --- Branch Instructions ---
  beq   x5, x5, beq_taken
  addi  x20, x0, 1
beq_taken:
  bne   x5, x6, bne_taken
  addi  x21, x0, 1
bne_taken:
  blt   x6, x5, blt_taken
  addi  x22, x0, 1
blt_taken:
  bge   x5, x6, bge_taken
  addi  x23, x0, 1
bge_taken:
  bltu  x6, x5, bltu_taken
  addi  x24, x0, 1
bltu_taken:
  bgeu  x5, x6, bgeu_taken
  addi  x25, x0, 1
bgeu_taken:

  # --- Jump Instructions (JAL, JALR) ---
  jal   x1, subroutine      # Jump to 'subroutine', store return PC in x1 (ra)

  # This code executes AFTER the subroutine returns.
  # We immediately jump to the end to halt the program.
  j     end_loop

# --- Subroutine Definition ---
# This code is now placed outside the main execution path.
# It is only executed when explicitly jumped to.
subroutine:
  addi  x27, x0, 123        # Inside the subroutine, x27 = 123
  jalr  x0, x1, 0           # Jump back using the address stored in x1

# --- Halt Program ---
# Use FENCE and then ECALL to gracefully stop the emulator.
end_loop:
  fence    # Demonstrate that FENCE assembles and executes.
  ecall    # This will signal the emulator to halt.

# An infinite loop in case the halt mechanism fails.
# This should not be reached anymore.
inf_loop:
  j inf_loop

# --- DATA SECTION ---
.data
.align 2 # Align to a 4-byte boundary
test_data:
  .word 0xDEADBEEF
store_target:
  .space 8
