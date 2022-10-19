#include <stdio.h>
#include <ctype.h>

#include "lex-anal.h"

char lexAnal(char test){
    return test;
}

int checkProlog(FILE *fp){
    return 0;
}

int scanner() {
    State current = Start;  // current state
    State previous;
    char in;

    switch (current)
    {
    case Start:
        if (in == '/') { previous = current; current = Slash; }
        break;
    case Slash:
        break;

    default:
        return 1; // unknown state
    }

    return 0; // everything allright
}
