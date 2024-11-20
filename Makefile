# SPDX-License-Identifier: MIT

TARGET_EXEC := tntouch
CC = gcc
BUILD_DIR := ./build
SRC_DIRS := ./src
CFLAGS := -Wall -Wextra -lm

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

$(BUILD_DIR)/$(TARGET_EXEC): $(SRCS) | $(BUILD_DIR)
	$(CC) $(SRCS) $(CFLAGS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
