#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include <stdio.h>

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
    Double,
    EulNum,
    EulNumExtra,
    EulDouble,
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
    MulSign,
    DotSign,
    GreaterThanSign,
    LesserThanSign,
    ExclamMark,
    ExclamEqual,
    GreaterEqualThanSign,
    DoubleEqual,
    StrictEquality,
    LesserEqualThanSign,
    NotEqual,
    EuldDouble,
} AutoState;

typedef struct 
{
    AutoState type; 
    char* lexeme;
    char* data; 
    int lineNum;
}Token;

typedef struct 
{
    Token **TokenArray;
    int length;
}TokenList;

int checkProlog(FILE* fp);

char* attachData(char *data, char dataToBeInserted);

Token* tokenCtor(AutoState type,int lineNum, char* State, char* data);

TokenList *appendToken(TokenList *list, Token* newToken);

Token* getToken(FILE* fp,int *lineNum);

void tokenDtor(Token *token);

void listDtor(TokenList*list);

TokenList *lexAnalyser(FILE *fp);


#endif // LEX_ANAL_H
