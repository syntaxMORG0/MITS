CC = gcc
CFLAGS = -Wall -Wextra -O2
BUILD_DIR = build
LIB_DIR = lib_mits
RUNTIME_DIR = runtime

# Source files
COMPILER_SRCS = $(LIB_DIR)/compiler.c $(LIB_DIR)/utils.c $(LIB_DIR)/rom.c $(LIB_DIR)/register.c
INTERPRETER_SRC = $(RUNTIME_DIR)/interpreter.c

# Build outputs
COMPILER_BIN = $(BUILD_DIR)/mits-compiler
INTERPRETER_BIN = $(BUILD_DIR)/mits-interp
LAUNCHER_BIN = $(BUILD_DIR)/mits
ROOT_LAUNCHER = mits

all: $(BUILD_DIR) $(COMPILER_BIN) $(INTERPRETER_BIN) $(LAUNCHER_BIN) $(ROOT_LAUNCHER)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(COMPILER_BIN): $(COMPILER_SRCS)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -o $@ $(COMPILER_SRCS)

$(INTERPRETER_BIN): $(INTERPRETER_SRC)
	$(CC) $(CFLAGS) -o $@ $(INTERPRETER_SRC)

# Create a simple launcher script that calls the interpreter
$(LAUNCHER_BIN): $(INTERPRETER_BIN)
	@echo '#!/bin/bash' > $@
	@echo 'SCRIPT_DIR="$$(cd "$$(dirname "$$0")" && pwd)"' >> $@
	@echo 'exec "$$SCRIPT_DIR/mits-interp" "$$@"' >> $@
	@chmod +x $@

# Create root-level launcher
$(ROOT_LAUNCHER): $(LAUNCHER_BIN)
	@echo '#!/bin/bash' > $@
	@echo 'SCRIPT_DIR="$$(cd "$$(dirname "$$0")" && pwd)"' >> $@
	@echo 'exec "$$SCRIPT_DIR/build/mits-interp" "$$@"' >> $@
	@chmod +x $@

clean:
	rm -rf $(BUILD_DIR) $(ROOT_LAUNCHER)

install: all
	@echo "To use 'mits' command globally, run:"
	@echo "  sudo cp $(LAUNCHER_BIN) /usr/local/bin/mits"
	@echo "  sudo cp $(CLI_BIN) /usr/local/bin/mits-cli"
	@echo "  sudo cp $(INTERPRETER_BIN) /usr/local/bin/mits-interp"

.PHONY: all clean install

