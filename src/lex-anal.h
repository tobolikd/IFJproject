#ifndef LEX_ANAL_H
#define LEX_ANAL_H

// needed for tests to run properly
#ifdef __cplusplus
extern "C" {
#endif


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

// mock function only to verify test functionality
char lexAnal(char test);


// needed for tests to run properly
#ifdef __cplusplus
}
#endif

#endif // LEX_ANAL_H
