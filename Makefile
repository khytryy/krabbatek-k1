# Icarus Verilog
IVERILOG := iverilog
VVP      := vvp

SRC_DIR  := src
BUILD_DIR:= build
TARGET   := kscript

SRCS := $(shell find $(SRC_DIR) -name "*.sv")
OUT  := $(BUILD_DIR)/$(TARGET).vvp

CFLAGS := -g2012 -Wall -Iinclude

all: $(OUT)

$(OUT): $(SRCS)
	@mkdir -p $(BUILD_DIR)
	@echo "		IVER $@"
	@$(IVERILOG) $(CFLAGS) -o $@ $(SRCS)

run: $(OUT)
	@echo "Running sim..."
	@$(VVP) $(OUT)

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean