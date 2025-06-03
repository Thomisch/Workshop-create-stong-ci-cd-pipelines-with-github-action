CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCDIR = src
TESTDIR = tests
SOURCES = $(SRCDIR)/calculator.c
MAIN_SRC = $(SRCDIR)/main.c
TEST_SRC = $(TESTDIR)/test_calculator.c

# Cibles principales
all: main

main: $(SOURCES) $(MAIN_SRC)
	$(CC) $(CFLAGS) -o main $(MAIN_SRC) $(SOURCES)

test: $(SOURCES) $(TEST_SRC)
	$(CC) $(CFLAGS) -o test_runner $(TEST_SRC) $(SOURCES)
	./test_runner

clean:
	rm -f main test_runner

.PHONY: all test clean