#ifndef LEX_ANAL_H
#define LEX_ANAL_H

// needed for tests to run properly
#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    Error,
    Start,
    Slash,
    StarComment,
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
    EulNumExtra,
    Double,
    String,
    StringEnd,
    Semicolon,
    Assign,
    RPar,
    LPar,
    RCurl,
    LCurl,
    PlusSign,
    MinusSign,
    StarSign,
    LastState //gives me total number of states
} AutoState;

int scanner();

// mock function only to verify test functionality
char lexAnal(char test);


// needed for tests to run properly
#ifdef __cplusplus
}
#endif

#endif // LEX_ANAL_H
