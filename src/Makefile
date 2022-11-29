PROJECT=code-gen
SOURCE_DIR=./src/

SOURCES += lex-analyzer.c
SOURCES += syn-analyzer.c
SOURCES += sem-analyzer.c
SOURCES += preced-analyzer.c
SOURCES += code-gen.c
SOURCES += code-gen-static.c
SOURCES += ast.c
SOURCES += stack.c
SOURCES += symtable.c
SOURCES += devel.c
SOURCES += test.c

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

.PHONY: all run tests clean compile purge valgrind compile_tests

all: clean compile

compile:
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROJECT)

run: compile
	./$(PROJECT)

valgrind: compile
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(PROJECT) || true

clean:
	rm -f $(PROJECT)

