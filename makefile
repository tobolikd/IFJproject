PROJECT=ifj
SCAN=$(PROJECT)-scanner
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -fcommon

.PHONY: all run tests clean

all: $(SCAN) 

$(SCAN):
	$(CC) $(CFLAGS) $(SCAN).c -o $(SCAN) 

run: $(SCAN)
	./$(SCAN)

clean:
	@rm -f $(SCAN)
