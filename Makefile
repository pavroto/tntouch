TARGET_EXEC := tntouch
CC = gcc
BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

$(BUILD_DIR)/$(TARGET_EXEC):
	$(CC) $(SRCS) -o $@
