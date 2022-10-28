#ifndef LEX_ANALYZER_H
#define LEX_ANALYZER_H

#include <stdio.h>

typedef enum
{
    EndOfProgram,
    Start,
    Slash,
    StarComment,
    LineComment,
    CommentStar,
    CommentEnd,
    QuestionMark,
    AlmostEndOfProgram,
    ID,
    DollarSign,
    VarID,
    Int,
    Dot,
    DotDouble,
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
    Comma,
    Colons,
    Backslash,
    nullType
} AutoState;

typedef struct 
{
    AutoState type; //intiger number which represents lexeme.
    char* lexeme; //what lexeme it represents.
    char* data; //additional data for some tokens.
    int lineNum; //line on which was token found in a given file. 
}Token;

/// @brief Structure to keep information about a length of array of tokens.
typedef struct 
{
    Token **TokenArray; //array of tokens.
    int length; //length of the list.
}TokenList;


/// @brief Function checks if string of characters <?php follows.
/// @param fp Pointer to a readable file.
/// @return 0 if <?php is present, if not return 1.
int checkProlog(FILE* fp);

/// @brief Appends 1 character to character array.
/// @param data Array of characters.
/// @param dataToBeInserted Character to be inserted.
/// @return Pointer to string with character added.
char* appendChar(char *data, char dataToBeInserted);

/// @brief Creates token.
/// @param type Autostate of token.
/// @param lineNum On what line was token found.
/// @param lexeme Token name.
/// @param data Additional data of token.
/// @return Pointer to newly created token.
Token* tokenCtor(AutoState type,int lineNum, char* lexeme, char* data);

/// @brief Appends token to an array of tokens.
/// @param list Array of tokens.
/// @param newToken Token to be inserted.
/// @return Pointer to array with newly appended token.
TokenList *appendToken(TokenList *list, Token* newToken);

/// @brief Searches through file and looks for a valid lexical token.
/// @param fp Pointer to file to scan through.
/// @param lineNum Pointer to line number where the file is currently reading from.
/// @return Pointer to first found token. Moves file cursor just after the read token.
/// @return Null if lexical error occured & token EndOfProgram if ended correctly
Token* getToken(FILE* fp,int *lineNum);

/// @brief Frees memory of tokens data and token itslef.
/// @param token 
void tokenDtor(Token *token);

/// @brief Frees token array with its data and tokenList itself.
/// @param list 
void listDtor(TokenList*list);

/// @brief Searches file for tokens. Saves them gradually in token array.
/// @param fp Pointer to a searched file.
/// @return Pointer to TokenList with every found tokens in an array of tokens with its additional data. NULL if data were not recognized or fail. 
TokenList *lexAnalyser(FILE *fp);

/// @brief Print out given token. [LineNum Lexeme Data]
/// @param token Token to be prited out.
void printToken(Token*token);

/// @brief Print out given token array. [LineNum Lexeme Data]
/// @param list List to be printed out.
void prinTokenList(TokenList *list);


#endif // LEX_ANALYZER_H
