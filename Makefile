CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude

TARGET := libcc.a
BIN := cc

SRC := $(filter-out src/main.c, $(wildcard src/*.c))
OBJ := $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET) $(BIN)

$(TARGET): $(OBJ)
	ar rcs $@ $(OBJ)

$(BIN): src/main.o $(TARGET)
	$(CC) $(CFLAGS) src/main.o $(TARGET) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/main.o $(TARGET) $(BIN)
