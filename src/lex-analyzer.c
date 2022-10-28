
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lex-analyzer.h"
#include "error-codes.h"

//check if prolog is present
int checkProlog(FILE* fp)
{
    char *prolog = "<?php";
    if(prolog[0]==fgetc(fp) )
        if(prolog[1]==fgetc(fp))
            if(prolog[2]==fgetc(fp))
                if(prolog[3]==fgetc(fp))  
                    if(prolog[4]==fgetc(fp))
                    {
                        char c = fgetc(fp);
                        ungetc(c,fp); //if end of line i want to catch it in KA
                        if (c=='\n' || c ==EOF || c == ' ')
                            return 0;
                    }
#if (DEBUG == 1)
    printf("%s : prolog is missing",prolog);
#endif
    return 1;                   
}

//allocate memory for data
//return new pointer
char* appendChar(char *data, char dataToBeInserted)
{
    int len;
    if (data == NULL)
        len = 1;
    else
        len = strlen(data)+1; // length of string + 1 + terminating null byte ('\0')
    
    data = realloc(data,(len+1)*sizeof(char));
    if (data == NULL) // error alocating memory
        return NULL;
    
    data[len-1] = dataToBeInserted;
    data[len] = '\0';
    return data;
}


//setup token
//return new token
Token* tokenCtor(AutoState type, int lineNum, char* lexeme, char* data)
{
    Token *new = malloc(sizeof(Token)); //allocates memory for new token
    if (new == NULL) // error alocating memory
        return NULL;
    new->data = data; //string already allocated
    new->lexeme = lexeme;
    new->lineNum = lineNum;
    new->type = type;
    return new;
}

//appends token to list of tokens
TokenList *appendToken(TokenList *list, Token* newToken)
{
    //new tokenList - initiate
    if(list == NULL)
    {
        list = malloc(sizeof(TokenList));
        debug_print("allocated list %p\n", (void *) list);
        list->TokenArray = malloc(2*sizeof(Token));
        list->length = 1;
    }
    else
    {
        //allocate memory for new token 
        list->length++;
        list->TokenArray = realloc(list->TokenArray,(list->length+1)*sizeof(Token));
    }
    if (list->TokenArray == NULL) // error allocating memory
        return NULL;
    list->TokenArray[list->length-1] = newToken;
    list->TokenArray[list->length] = NULL;
    return list;
}

/// @brief Checks if datas value is one of valide data types in php.
/// @param data String to be checked.
/// @return Returns 1 if data represents data type. Returns 0 if id doesnt.
int checkDataType(char *data)
{
    if (data == NULL)
        return 0;
    if (!strcmp(data,"string"))
        return 1;
    else if (!strcmp(data,"int"))
        return 1;
    else if (!strcmp(data,"float"))
        return 1;
    return 0;    
}


//returns pointer to setup token
//returns NULL if error accured
Token* getToken(FILE* fp,int *lineNum)
{
    int curState = Start;
    char curEdge = fgetc(fp);
    char *data = NULL;

    while (1) //until you get a token -> whole finite state 
    {
        if (curEdge == EOF)
        {
            switch (curState)
            {
            case Start:
                return tokenCtor(EndOfProgram,*lineNum, "EndOfProgram", data);

            case String:
                free(data);
                return NULL;

            case ID:
                return tokenCtor(ID,*lineNum, "ID", data);
            
            case StarComment:
                return NULL;

            case CommentStar:
                return NULL;

            case AlmostEndOfProgram:
                return tokenCtor(EndOfProgram,*lineNum, "EndOfProgram", data);

            case QuestionMark:
                if (checkDataType(data))
                    return tokenCtor(nullType, *lineNum, "nullType", data);
                free(data);
                return NULL;                

            default:
                break;
            }
        }
        
        switch (curState)
        {
        case Start:
            //white signs
            if (curEdge == ' ') 
                break;
            if (curEdge == '\n')//count number of lines for function foundToken & debug
            {
                *lineNum = *lineNum +1;
                break;
            }
            if (curEdge == '/')
            {
                curState = Slash; break;
            }
            if (curEdge == '?')
            {
                curState = QuestionMark; break;
            }
            if (isalpha(curEdge)|| curEdge == '_')
            {
                curState = ID;
                data = appendChar(data, curEdge);//attach the caller
                break;
            }
            if (isdigit(curEdge))
            {
                curState = Int; 
                data = appendChar(data, curEdge); //attach the caller
                break;
            }
            if (curEdge == '$')
            {
                curState = DollarSign; break;
            }
            if (curEdge == '"')
            {
                curState = String; break;
            }
            if (curEdge == '=')
            {
                curState = Assign;
                break;
            }
            if (curEdge =='>')
            {
                curState = GreaterThanSign; break;
            }
            if (curEdge =='<')
            {
                curState = LesserThanSign; break;
            }
            if (curEdge =='!')
            {
                curState = ExclamMark; break;
            }
            if (curEdge == ';')
                return tokenCtor(Semicolon,*lineNum, "Semicolon", data);
            if (curEdge == ')')
                return tokenCtor(RPar, *lineNum, "RPar", data);
            if (curEdge == '(')
                return tokenCtor(LPar,*lineNum, "LPar", data);
            if (curEdge == '}')
                return tokenCtor(RCurl, *lineNum, "RCurl", data);
            if (curEdge == '{')
                return tokenCtor(LCurl,*lineNum, "LCurl", data); 
            if (curEdge =='*')
                return tokenCtor(MulSign,*lineNum, "MulSign", data);
            if (curEdge =='-')
                return tokenCtor(MinusSign,*lineNum, "MinusSign", data);
            if (curEdge =='+')
                return tokenCtor(PlusSign,*lineNum, "PlusSign", data);
            if (curEdge =='.') 
                return tokenCtor(DotSign,*lineNum, "DotSign", data);
            if (curEdge ==',')
                return tokenCtor(Comma,*lineNum, "Comma", data);
            if (curEdge ==':')
                return tokenCtor(Colons,*lineNum, "Colons", data);
            // if (curEdge =='\\')
            //     return tokenCtor(Backslash,*lineNum, "Backslash", data);
            

            #if (DEBUG == 1)
                printf("At line: %d START -> %c : Not recognised",*lineNum,curEdge);
            #endif
            
            return NULL;
            break;

        case Slash:
            if (curEdge == '/')
            {
                curState = LineComment; break;
            }
            if (curEdge == '*')
            {
                curState = StarComment; break;
            }
            ungetc(curEdge,fp); //catch the "force-out" edge
            return tokenCtor(Slash,*lineNum, "Slash",data);            

        case LineComment:
            if (curEdge == '\n')
            {
                *lineNum = *lineNum + 1;
                curState = Start;
            }
            break;

        case StarComment:
            if (curEdge == '\n')
                *lineNum = *lineNum +1;
            if (curEdge == '*')
                curState = CommentStar;
            break;

        case CommentStar:
            if (curEdge == '/')
            {
                curState = Start;
                break;
            }
            if (curEdge == '\n')
                *lineNum = *lineNum +1;
            else
                curState = StarComment;
            break;

        case QuestionMark:
            if (curEdge == '>')
            {
                curState = AlmostEndOfProgram; 
                break;
            }
            else if (isprint(curEdge))//look for data type
            {
                data = appendChar(data, curEdge);
                break;
            }
            if (checkDataType(data))//returns 1 it is a valid data type
            {
                ungetc(curEdge,fp); //catch the "force-out" edge
                return tokenCtor(nullType, *lineNum, "nullType", data);
            }
            return NULL;
        
        case AlmostEndOfProgram:
            return NULL; //nothing should come after ?>    

        case ID:
            if (isalnum(curEdge) || curEdge == '_')  //alphanumeric or punctuation mark
                data = appendChar(data, curEdge);
            else
            {
                ungetc(curEdge,fp);
                return tokenCtor(ID,*lineNum, "ID", data);
            }
            break;

        case DollarSign:
            if (isalpha(curEdge) || curEdge == '_')
            {
                curState = VarID;
                data = appendChar(data, curEdge); //attach the caller
                break;
            }
            return NULL; //dollar sign isnt final state 
            
        case VarID:
            if (isalpha(curEdge) || curEdge == '_')
            {
                data = appendChar(data, curEdge);
                break;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(VarID,*lineNum, "VarID", data);

        case String:
            if (curEdge == '"')
            {
                if (data != NULL)
                {
                    int cursor = strlen(data)-1; //loop through the array
                    int count = 0; //number of \\bs

                    while (cursor > 1) // solve repeating \\bs
                    {
                        if(data[cursor] != '\\') // in case there was \\bs before
                            break;
                        count++;
                        cursor--;
                    }

                    if (count%2) //if there is odd number of \\bs -> put " to string
                    {
                        data[strlen(data)-1] = '"';
                        break;
                    }
                }
                return tokenCtor(StringEnd,*lineNum, "StringEnd", data);
            }
            else if (isprint(curEdge))
            {
                data = appendChar(data, curEdge);
                break;
            }
            //weird data on the input
            #if (DEBUG == 1)
                printf("At line: %d STRING -> %c : Not recognised",*lineNum,curEdge);
            #endif

            return NULL;

        case Assign:
            if (curEdge == '=')
            {
                curState = DoubleEqual; break;
            }
            return tokenCtor(Assign,*lineNum, "Assign", data);
            
        case DoubleEqual:
            if (curEdge == '=')
                return tokenCtor(StrictEquality,*lineNum, "StrictEquality", data);
            #if (DEBUG == 1)
                printf("Line %d c %c- Lexical Error", *lineNum,curEdge);
            #endif
            return NULL;

        //numbers
        case Int:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            if (curEdge == '.')
            {
                data = appendChar(data,curEdge);
                curState = Double;
                break;
            }
            if (curEdge == 'e' || curEdge == 'E')
            {
                data = appendChar(data,curEdge);
                curState = EulNum;
                break;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(Int,*lineNum, "Int", data);

        case Double:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            if (curEdge == 'e' || curEdge == 'E')
            {
                data = appendChar(data,curEdge);
                curState = EulNum;
                break;
            }
            //if there is anything else...
            //but if there is nothing after dot means error
            if (data[strlen(data)-1] == '.')
            {
                #if (DEBUG == 1)
                    printf("Line %d - Number cannot end with . sign.",*lineNum);
                #endif
                return NULL;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(Double,*lineNum, "Double", data);

        case EulNum:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                curState = EulDouble;
                break;
            }
            if (curEdge == '+' || curEdge == '-')
            {
                data = appendChar(data,curEdge);
                curState = EulNumExtra;
                break;
            }
            //cannot end with e as its last character
            return NULL;

        case EulNumExtra:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                curState = EulDouble;
                break;
            }
		#if (DEBUG == 1)
            printf("Line %d - Number cannot end with . sign.",*lineNum);
		#endif
            return NULL; //return error

        case EulDouble:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            //previous state == EulNum
            if (data[strlen(data)-1] == 'e' || data[strlen(data)-1] == 'E' )
            {
                #if (DEBUG == 1)
                    printf("Line %d - %c Unpropper double number ending.",*lineNum,curEdge);
                #endif
                return NULL;
            }
            //previous state == EulNUmExtra
            if (data[strlen(data)-1] == '+' || data[strlen(data)-1] == '-' )
            {
                #if (DEBUG == 1)
                    printf("Line %d - %c Unpropper double number ending.",*lineNum,curEdge);
                #endif
                return NULL;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(EuldDouble,*lineNum, "EuldDouble", data);
        //end of numbers

        case ExclamMark:
            if (curEdge == '=')
            {
                curState = ExclamEqual;break;
            }
            #if (DEBUG == 1)
                printf("Line %d - %c Lexical error.",*lineNum,curEdge);
            #endif
            return NULL; //return error

        case ExclamEqual:
            if (curEdge == '=')
                return tokenCtor(NotEqual,*lineNum, "NotEqual", data);
            #if (DEBUG == 1)
                printf("Line %d - %c Lexical error.",*lineNum,curEdge);
            #endif
            return NULL; //return error
            
        case GreaterThanSign:
            if (curEdge == '=')
                return tokenCtor(GreaterEqualThanSign,*lineNum, "GreaterEqualThanSign", data);
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(GreaterThanSign,*lineNum, "GreaterThanSign", data);

        case LesserThanSign:
            if (curEdge == '=')
                return tokenCtor(LesserEqualThanSign,*lineNum, "LesserEqualThanSign", data);
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(LesserThanSign,*lineNum, "LesserThanSign", data);

        default:
            break;

        } // end of switch 

        if(curEdge == EOF && curState != Start)
        {
            if (data != NULL)
                free(data);
            return NULL;
        }
        curEdge = fgetc(fp); //get another edge
    }
#if (DEBUG == 1)
    printf("unthinkable happend"); // should never happen
#endif
    return NULL;
}

void tokenDtor(Token *token)
{
    debug_print("deconstructing token\n");
    if (token != NULL)
    {
        if (token->data != NULL)
        {
            debug_print("freeing token->data...");
            free(token->data);
            debug_print("done\n");
        }
        debug_print("freeing token...");
        free(token);
        debug_print("done\n");
    }
    debug_print("token deconstructed\n");
}

void listDtor(TokenList *list)
{
    debug_print("freeing list %p\n", (void *) list);
    if (list != NULL)
    {
        for (int i = list->length; i >= 0; i--)
        {
            if (list->TokenArray[i] != NULL)
                tokenDtor(list->TokenArray[i]);
        }
    
        if (list->TokenArray != NULL)
            free(list->TokenArray);
        free(list);
    }
    debug_print("list %p freed\n", (void *) list);
}

/* TODO */
Token *checkForKeyWord(Token *token)
{
    return token;
}

void printToken(Token*token)
{
    if (token->data == NULL)
        printf("%d %s\n",token->lineNum,token->lexeme);
    else
        printf("%d %s %s\n",token->lineNum,token->lexeme, token->data);
}


void prinTokenList(TokenList *list)
{
    for (int i = 0; i < list->length; i++)
    {
        if (list->TokenArray[i] != NULL)
            printToken(list->TokenArray[i]);
    }
}

//NULL to propagate error
TokenList *lexAnalyser(FILE *fp)
{
    if (checkProlog(fp))
        return NULL;
    
    TokenList *list = NULL; //structure to string tokens together
    Token* curToken = NULL; //token cursor

    int lineNum = 1; //on what line token is found
                     //starts on line number (prolog is on line 1)

    //loop through the program - get & append tokens
    while (1)
    {
        curToken = getToken(fp,&lineNum); //get token

        //getToken returned NULL this means error ! 
        if (curToken == NULL) 
        {
            listDtor(list);
            return NULL;
        }
        
        //check for keywords if lexeme == id
        if (curToken->type == ID)
        {
            curToken = checkForKeyWord(curToken);
        }        

        //EOF - dont append this token
        if (curToken->type == EndOfProgram)
        {
            free(curToken);
            break;
        }

        list = appendToken(list,curToken); //append to list
    }

    return list;
}
