CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

TARGET = ros2_dds_simulator
TEST_TARGET = test_simulator

SRC = src/main.c src/simulator.c
TEST_SRC = tests/test_simulator.c src/simulator.c

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

run: $(TARGET)
	.\$(TARGET)

test: $(TEST_TARGET)
	.\$(TEST_TARGET)

clean:
	del /Q $(TARGET).exe 2>nul
	del /Q $(TEST_TARGET).exe 2>nul
