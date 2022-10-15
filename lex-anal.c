#include <stdio.h>
#include <ctype.h>

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
} States;

void foo() {
    int d;  // temporary var for get rid of errors
    switch (d)
    {
    case Start:
        if (d == "/") return Slash;
    case Slash:
        return;
    }
}
