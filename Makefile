# SPDX-License-Identifier: MIT

TARGET_EXEC := tntouch
CC = gcc
BUILD_DIR := ./build
SRC_DIRS := ./src
CFLAGS := -Wall -Wextra

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

$(BUILD_DIR)/$(TARGET_EXEC): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $@
