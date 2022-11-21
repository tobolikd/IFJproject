PROJECT=ifj-compiler
SOURCE_DIR=./src/

SOURCES += lex-analyzer.c
SOURCES += syn-analyzer.c
SOURCES += preced-analyzer.c
SOURCES += code-gen.c
SOURCES += ast.c
SOURCES += stack.c
SOURCES += symtable.c
SOURCES += devel.c
SOURCES += main.c

TARGETS = $(addprefix $(SOURCE_DIR), $(SOURCES))

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

.PHONY: all run tests clean compile purge valgrind compile_tests

all: clean compile

compile:
	$(CC) $(CFLAGS) $(TARGETS) -o $(PROJECT)

run: compile
	./$(PROJECT)

compile_tests:
	cmake -S . -B build
	cmake --build build

tests: compile_tests
	./build/tests-all || true

valgrind: compile_tests
	valgrind --leak-check=full --show-leak-kinds=all build/tests-all ./build/tests-all || true

clean:
	rm -f $(PROJECT)

purge: clean
	rm -fr build
