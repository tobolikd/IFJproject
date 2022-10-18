PROJECT=ifj
SCAN=lex-anal
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

.PHONY: all run clean

all: clean run

$(SCAN):
	$(CC) $(CFLAGS) $(SCAN).c -o $(SCAN)

run: $(SCAN)
	./$(SCAN)

clean:
	@rm -f *.o $(SCAN) 
