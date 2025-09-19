# Project settings
TARGET     := rv32i
SRC_DIR    := src
INC_DIR    := include
BUILD_DIR  := build
TEST_DIR   := test

# --- RISC-V Toolchain ---
# Assumes a RISC-V toolchain is in your PATH (e.g., riscv64-unknown-elf-)
RISCV_PREFIX  := riscv64-unknown-elf-
RISCV_AS      := $(RISCV_PREFIX)as
RISCV_LD      := $(RISCV_PREFIX)ld      # <-- NEW: Linker tool
RISCV_OBJCOPY := $(RISCV_PREFIX)objcopy
RISCV_ASFLAGS :=

# --- Assembly Program ---
ASM_SRC       := program.s
ASM_OBJ       := $(BUILD_DIR)/program.o
ASM_ELF       := $(BUILD_DIR)/program.elf # <-- NEW: Intermediate ELF file
ASM_BIN       := program.bin

# --- Emulator Build Tools ---
CC         := gcc
CXX        := g++
CFLAGS     := -Wall -Wextra -std=c11 -I$(INC_DIR) -g
CXXFLAGS   := -Wall -Wextra -std=c++14 -I$(INC_DIR) -g
LDFLAGS    :=
LDLIBS_GTEST := -lgtest -lgtest_main -pthread

# Source & object files
SRCS       := $(wildcard $(SRC_DIR)/*.c)
OBJS       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TEST_SRCS  := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS  := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))

# Default rule: Build the emulator executable
all: $(TARGET)

# --- Main Rules ---

# --- RULE FOR BUILDING program.bin ---
# This new rule uses the linker to ensure sections are ordered correctly.
$(ASM_BIN): $(ASM_SRC) | $(BUILD_DIR)
	@echo "  AS      $<"
	$(RISCV_AS) $(RISCV_ASFLAGS) -o $(ASM_OBJ) $<
	@echo "  LD      $(ASM_OBJ) -> $(ASM_ELF)"
	# --- THIS LINE IS MODIFIED ---
	$(RISCV_LD) -T linker.ld -o $(ASM_ELF) $(ASM_OBJ)
	@echo "  OBJCOPY $(ASM_ELF) -> $@"
	$(RISCV_OBJCOPY) -O binary $(ASM_ELF) $@

# Link the main emulator executable
$(TARGET): $(OBJS)
	@echo "  LD      $@"
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Run the emulator with the assembled program.
run: $(TARGET) $(ASM_BIN)
	@echo "  RUN     ./$(TARGET) (loading $(ASM_BIN))"
	./$(TARGET)

# Link the test executable
test_runner: $(filter-out $(BUILD_DIR)/main.o, $(OBJS)) $(TEST_OBJS)
	@echo "  LD      $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS_GTEST)

# Run tests
test: test_runner
	./test_runner

# --- Compilation Rules ---

# Compile C source files for the emulator
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "  CC      $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ test files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	@echo "  CXX     $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Utility Rules ---

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate compile_commands.json using bear
compile_commands.json:
	bear -- make $(TARGET)

# Clean up all build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET) test_runner $(ASM_BIN) compile_commands.json

.PHONY: all run clean test
