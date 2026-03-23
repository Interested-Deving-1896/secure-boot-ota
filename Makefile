
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic
DEBUG_CFLAGS = -g -O0 -DDEBUG
RELEASE_CFLAGS = -O2 -DNDEBUG

INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/test_%.o,$(TEST_SRCS))

TARGET = secure_boot_ota_demo
TEST_TARGET = secure_boot_ota_tests

.PHONY: all clean debug release test

all: release

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(TARGET)

release: CFLAGS += $(RELEASE_CFLAGS)
release: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

# Dependencies
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
$(BUILD_DIR)/secure_boot.o: $(SRC_DIR)/secure_boot.c
$(BUILD_DIR)/ota_manager.o: $(SRC_DIR)/ota_manager.c
$(BUILD_DIR)/crypto_utils.o: $(SRC_DIR)/crypto_utils.c
