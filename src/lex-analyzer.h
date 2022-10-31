#ifndef LEX_ANALYZER_H
#define LEX_ANALYZER_H

#include <stdio.h>
#include <string.h>

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_STATE(STATE)\
    STATE(Error)\
    STATE(Start)\
    STATE(Slash)\
    STATE(StarComment)\
    STATE(LineComment)\
    STATE(CommentStar)\
    STATE(CommentEnd)\
    STATE(QuestionMark)\
    STATE(AlmostEndOfProgram)\
    STATE(EndOfProgram)\
    STATE(ID)\
    STATE(DollarSign)\
    STATE(VarID)\
    STATE(Int)\
    STATE(Dot)\
    STATE(Double)\
    STATE(EulNum)\
    STATE(EulNumExtra)\
    STATE(EulDouble)\
    STATE(String)\
    STATE(StringEnd)\
    STATE(Semicolon)\
    STATE(Assign)\
    STATE(RPar)\
    STATE(LPar)\
    STATE(RCurl)\
    STATE(LCurl)\
    STATE(PlusSign)\
    STATE(MinusSign)\
    STATE(MulSign)\
    STATE(DotSign)\
    STATE(GreaterThanSign)\
    STATE(LesserThanSign)\
    STATE(ExclamMark)\
    STATE(ExclamEqual)\
    STATE(GreaterEqualThanSign)\
    STATE(DoubleEqual)\
    STATE(StrictEquality)\
    STATE(LesserEqualThanSign)\
    STATE(NotEqual)\
    STATE(EuldDouble)\
    STATE(Comma)\
    STATE(Colons)\
    STATE(Backslash)\

typedef enum
{
    FOREACH_STATE(GENERATE_ENUM) 
} AutoState;

// get state string by using STATE_STRING[AutoState]
extern const char *STATE_STRING[];

#define FOREACH_TOKEN_TYPE(TYPE)\
        TYPE(t_EOF)/*data - <NULL>*/\
        TYPE(t_if)/*data - <NULL>*/\
        TYPE(t_while)/*data - <NULL>*/\
        TYPE(t_else)/*data - <NULL>*/\
        TYPE(t_null)/*data - <NULL>*/\
        TYPE(t_return)/*data - <NULL>*/\
        TYPE(t_function)/*data - <NULL>*/\
        TYPE(t_type)/*data - int | string | float | void*/\
        TYPE(t_nullType)/*data - int | string | float*/\
        TYPE(t_varId)/*data - <var name>*/\
        TYPE(t_functionId)/* - <fnc name>*/\
        TYPE(t_operator)/*data - . | + | - | * | / */\
        TYPE(t_comparator)/*data - === | !== | < | > | <= | >= */\
        TYPE(t_int)/*data - <int value>*/\
        TYPE(t_float)/*data - <float value>*/\
        TYPE(t_string)/*data - <string value> | <NULL> */\
        TYPE(t_semicolon)/*data - <NULL>*/\
        TYPE(t_assign)/*data - <NULL>*/\
        TYPE(t_lPar)/*data - <NULL>*/\
        TYPE(t_rPar)/*data - <NULL>*/\
        TYPE(t_lCurl)/*data - <NULL>*/\
        TYPE(t_rCurl)/*data - <NULL>*/\
        TYPE(t_comma)/*data - */\
        TYPE(t_colon)/*data - */\

typedef enum
{
   FOREACH_TOKEN_TYPE(GENERATE_ENUM) 
} TokenType;

// get token type string by using TOKEN_TYPE_STRING[TokenType]
extern const char *TOKEN_TYPE_STRING[];

typedef struct 
{
    TokenType type; // token type represented by enum TokenType
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

/// @brief Compares tokens data to known keywords.
/// @param token Token with type t_functionId.
/// @return Token with updated tokenType and data if keyword is found.
Token *getKeyword(Token *token);

/// @brief Appends 1 character to character array.
/// @param data Array of characters.
/// @param dataToBeInserted Character to be inserted.
/// @return Pointer to string with character added.
char *appendChar(char *data, char dataToBeInserted);

/// @brief Creates token.
/// @param type Autostate of token.
/// @param lineNum On what line was token found.
/// @param lexeme Token name.
/// @param data Additional data of token.
/// @return Pointer to newly created token.
Token *tokenCtor(TokenType type, int lineNum, char* data);

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
Token *getToken(FILE* fp,int *lineNum);

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
void printToken(Token *token);

/// @brief Print out given token array. [LineNum Lexeme Data]
/// @param list List to be printed out.
void prinTokenList(TokenList *list);


#endif // LEX_ANALYZER_H
