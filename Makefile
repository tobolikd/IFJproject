PROJECT=ifj-compiler
SOURCE_DIR=./src/

SOURCES += lex_analyzer.c
SOURCES += syn_analyzer.c
SOURCES += sem_analyzer.c
SOURCES += preced_analyzer.c
SOURCES += code_gen.c
SOURCES += code_gen_static.c
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
	valgrind -s --leak-check=full --show-leak-kinds=all build/tests-all ./build/tests-all || true

clean:
	rm -f $(PROJECT)

out: compile
	cat myTestFiles/test.php | ./ifj-compiler

purge: clean
	rm -fr build
