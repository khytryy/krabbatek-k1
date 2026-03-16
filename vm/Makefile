CC      := gcc
CFLAGS  := -Wall -Wextra  -g -O2 -I include -MMD -MP

SRC_DIR := src
OBJ_DIR := obj
TARGET  := ktek-k1-vm

SRCS    := $(shell find $(SRC_DIR) -name "*.c")
INC     := $(shell find include -name "*.h")
OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS    := $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "     LD $@"
	@$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "     CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJ_DIR) $(TARGET) build

format:
	@echo "Formatting code..."
	@clang-format -i $(SRCS) $(INC)

.PHONY: all clean format

-include $(DEPS)