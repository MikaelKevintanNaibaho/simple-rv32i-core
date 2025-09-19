# Simple RV32I Core

This project is a simple emulator for the **RISC-V 32-bit integer instruction set (RV32I)**.  
It is written in **C**.

---

## Features

- **RV32I base Instruction Set**  
  Implements a wide range of RV32I instructions, including:
  - **Integer Computation**: `ADDI`, `SLTI`, `SLTIU`, `XORI`, `ORI`, `ANDI`, `SLLI`, `SRLI`, `SRAI`
  - **Control Transfer**: `LUI`, `AUIPC`, `JAL`, `JALR`, `BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU`
  - **Load and Store**: `LB`, `LH`, `LW`, `LBU`, `LHU`, `SB`, `SH`, `SW`
  - **System**: `ECALL`, `EBREAK`, `FENCE`

- **CPU and Memory Emulation**  
  Provides basic emulation of registers and memory.

---

## Getting Started

### Prerequisites

Make sure you have the following installed:

- **RISC-V GCC Toolchain**
  Install via your system package manager:

  ```bash
  sudo apt install gcc-riscv64-unknown-elf
  ```

- **GCC and G++**  
  Used to build the emulator:

- **Google Test**
  Used to build the emulator:
  ```bash
  sudo apt install libgtest-dev
  ```


### Installation

- Clone the repository:
```bash
git clone https://github.com/MikaelKevintanNaibaho/simple-rv32i-core.git
cd simple-rv32i-core
```

### Usage

- **Build the Emulator**
```bash
make
```

- **Assemble a Sample Program**
```bash
make program.bin
```

- **Run the Emulator**
```bash
make run
```
or:
```bash
./rv32i example.bin
```
example using the sample program:
```bash
./rv32i program.bin
```


