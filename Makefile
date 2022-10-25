PROJECT=ifj-compiler
SOURCE_DIR=./src/

SOURCES += lex-analyzer.c
SOURCES += syn-analyzer.c
SOURCES += code-gen.c
SOURCES += main.c

TARGETS = $(addprefix $(SOURCE_DIR), $(SOURCES))

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

.PHONY: all run tests clean compile

all: compile

compile:
	$(CC) $(CFLAGS) $(TARGETS) -o $(PROJECT) 

run: compile
	./$(PROJECT)

tests: 
	cmake -S . -B build
	cmake --build build
	./build/tests-all || true

clean:
	@rm -fr build $(PROJECT)
