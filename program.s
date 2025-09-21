# program.s
# A comprehensive test program for the RV32I emulator, with a logical execution flow.
# All instruction tests run first, followed by a syscall demonstration, and then a clean exit.

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
  li    x1, 0x0800000F
  li    x2, 4
  li    x3, 1
  sll   x4, x1, x2
  slt   x10, x1, x3
  sltu  x11, x1, x3

  # --- M-Extension: Multiplication ---
  addi x5, x0, 8
  addi x6, x0, 7
  mul  x7, x5, x6

  # --- M-Extension: Division ---
  addi x8, x0, 100
  addi x9, x0, 10
  div  x10, x8, x9

  # --- I-Type: Shift Instructions ---
  lui   x20, 0x08000
  addi  x20, x20, 15
  slli  x21, x20, 4
  srli  x22, x20, 4
  srai  x23, x20, 4

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
  jal   x1, subroutine

  # --- Syscall Demonstration ---
  # This code runs after the subroutine and atomic tests are complete.
  li    a7, 1      # Syscall number for "print char"
  li    a0, 'H'
  ecall
  li    a0, 'e'
  ecall
  li    a0, 'l'
  ecall
  ecall            # Print 'l' again
  li    a0, 'o'
  ecall
  li    a0, ','
  ecall
  li    a0, ' '
  ecall
  li    a0, 'w'
  ecall
  li    a0, 'o'
  ecall
  li    a0, 'r'
  ecall
  li    a0, 'l'
  ecall
  li    a0, 'd'
  ecall
  li    a0, '!'
  ecall
  li    a0, '\n'
  ecall

  # --- Halt Program ---
  # This is the final step. Use the standard RISC-V exit ecall.
end_program:
  li a7, 93    # ecall code for exit
  li a0, 0     # exit code 0
  ecall        # This will signal the emulator to halt.

# --- Subroutine and Test Definitions ---
# These are placed out of the main execution flow and are only reached by jumps.
subroutine:
  addi  x27, x0, 123
  jal   x0, amo_test  # Jump to the atomic test section
  jalr  x0, x1, 0     # Jump back using the address stored in x1 (ra)

amo_test:
  la    x10, amo_data
  li    x11, 5
  amoswap.w x12, x11, (x10)
  amoadd.w  x13, x11, (x10)
  jr    ra # Return from amo_test using the return address

# --- DATA SECTION ---
.data
.align 2 # Align to a 4-byte boundary
test_data:
  .word 0xDEADBEEF
store_target:
  .space 8
amo_data:
  .word 0
