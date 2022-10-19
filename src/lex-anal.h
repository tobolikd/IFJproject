#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include <stdio.h>

typedef enum
{
    Start,
    Slash,
    Comment,
    LineComment,
    CommentStar,
    CommentEnd,
    QuestionMark,
    AlmostEndOfProgram,
    EndOfProgram,
    ID,
    DollarSign,
    VarID,
    Int,
    Dot,
    DoubleDot,
    EulNum,
    Double,
    String,
    StringEnd,
    Semicolon,
    Assign,
    RPar,
    LPar,
    PlusSign,
    MinusSign,
    StarSign,
    Error
} State;

int scanner();

/* reads from fp and checks if it starts with "<?php"
 *
 * note: no whitespace characters aren't allowed before prolog
 * @param fp file descriptor to read from
 */
int checkProlog(FILE *fp);

// mock function only to verify test functionality
char lexAnal(char test);

#endif // LEX_ANAL_H
