CC ?= gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
SAN_FLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -g -O1 -fsanitize=address,undefined
LDFLAGS := -lm

BUILD_DIR := build
APP_BIN := $(BUILD_DIR)/bilheteria_cinema
TEST_BIN := $(BUILD_DIR)/test_bilheteria
SAN_TEST_BIN := $(BUILD_DIR)/test_bilheteria_san

.PHONY: all run interactive test sanitize clean

all: $(APP_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP_BIN): src/main.c src/bilheteria.c src/bilheteria.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/main.c src/bilheteria.c -o $@

$(TEST_BIN): tests/test_bilheteria.c src/bilheteria.c src/bilheteria.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_bilheteria.c src/bilheteria.c -o $@ $(LDFLAGS)

$(SAN_TEST_BIN): tests/test_bilheteria.c src/bilheteria.c src/bilheteria.h | $(BUILD_DIR)
	$(CC) $(SAN_FLAGS) tests/test_bilheteria.c src/bilheteria.c -o $@ $(LDFLAGS)

run: $(APP_BIN)
	./$(APP_BIN)

interactive: $(APP_BIN)
	./$(APP_BIN) --interativo

test: $(APP_BIN) $(TEST_BIN)
	./$(TEST_BIN)
	bash tests/test_cli.sh ./$(APP_BIN)

sanitize: $(SAN_TEST_BIN)
	./$(SAN_TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
