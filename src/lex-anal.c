#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lex-anal.h"

//check if prolog is present
int checkProlog(FILE* fp)
{
    char *prolog = "<?php";
    if(prolog[0]==fgetc(fp) )
        if(prolog[1]==fgetc(fp))
            if(prolog[2]==fgetc(fp))
                if(prolog[3]==fgetc(fp))  
                    if(prolog[4]==fgetc(fp))
                        return 0;
    //DEBUG
    printf("%s : prolog is missing",prolog);
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

/* TODO error stavy */
/* TODO EOF edge v jednolivych stavoch*/
//returns pointer to setup token
//returns NULL if error accured
Token* getToken(FILE* fp,int *lineNum)
{
    int curState = Start;
    char curEdge = fgetc(fp); //reads from file GLOBAL POINTER
    char *data=NULL;

    while (1) //until you get a token -> whole finite state 
    {
        if (curEdge == EOF && curState != AlmostEndOfProgram)
        {
            free(data);
            return tokenCtor(Error,*lineNum, "EOF", NULL);
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
            if (isalpha(curEdge))
            {
                curState = ID;
                // data = appendChar(data, curEdge);//attach the caller
                fseek(fp,-1,SEEK_CUR); break;//catch the "force-out" edge
            }
            if (curEdge >= '0' && curEdge <= '9')
            {
                curState = Int; 
                // data = appendChar(data, curEdge); //attach the caller
                fseek(fp,-1,SEEK_CUR); break;//catch the "force-out" edge
            }
            if (curEdge == '$')
            {
                curState = DollarSign; break;
            }
            if (curEdge == '"')
            {
                data = appendChar(data, curEdge);
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
            if (curEdge =='.')
                return tokenCtor(DotSign,*lineNum, "DotSign", data);
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
                return tokenCtor(Colon,*lineNum, "Colon", data);
            if (curEdge =='\\')
                return tokenCtor(Backslash,*lineNum, "Backslash", data);
            

            if (curEdge == EOF)
                return tokenCtor(Error,*lineNum, "EOF", NULL);
            
            //DEBUG
            printf("At line: %d START -> %c : Not recognised",*lineNum,curEdge);
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
            fseek(fp,-1,SEEK_CUR); break;//catch the "force-out" edge
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
            if (curEdge == '\n')
                *lineNum = *lineNum +1;
            if (curEdge == '/')
            {
                curState = Start;
            }
            else
                curState = StarComment;
            break;

        case QuestionMark:
            if (curEdge == '>')
            {
                curState = AlmostEndOfProgram; break;
            }
            else
            {
                fseek(fp,-1,SEEK_CUR);
                return tokenCtor(QuestionMark,*lineNum, "QuestionMark", data);
            }
            break;

        case ID:
            if (isalnum(curEdge) || curEdge == '_')  //alphanumeric or punctuation mark
                data = appendChar(data, curEdge);
            else
            {
                fseek(fp,-1,SEEK_CUR);
                return tokenCtor(ID,*lineNum, "ID", data);
            }
            break;

        case DollarSign:
            if (isalpha(curEdge))
            {
                curState = VarID;
                // data = appendChar(data, curEdge); //attach the caller
                fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
            }
            else
            {
                return tokenCtor(DollarSign,*lineNum, "DollarSign", data);
            }
            break;
            
        case VarID:
            if (isalnum(curEdge) || curEdge == '_')
                data = appendChar(data, curEdge);
            else
            {
                return tokenCtor(VarID,*lineNum, "VarID", data);
            }
            break;

        case String:
            /* TODO - possibly more signs from ASCII may cause error when inside string*/
            if (curEdge == '"')
            {
                data = appendChar(data, curEdge);
                return tokenCtor(StringEnd,*lineNum, "StringEnd", data);
            }
            else if (isprint(curEdge))
            {
                data = appendChar(data, curEdge);
                break;
            }
            //DEBUG
            printf("At line: %d STRING -> %c : Not recognised",*lineNum,curEdge);
            return NULL;
            break;

        case Assign:
            if (curEdge == '=')
            {
                curState = DoubleEqual; break;
            }
            return tokenCtor(Assign,*lineNum, "Assign", data);
            
        case DoubleEqual:
            if (curEdge == '=')
                return tokenCtor(StrictEquality,*lineNum, "StrictEquality", data);
            printf("Line %d - Lexical Error", *lineNum);
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
            fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
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
                printf("Line %d - Number cannot end with . sign.",*lineNum);
                return NULL;
            }
            fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
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
            if (data[strlen(data)-1] == 'e')
            {
                printf("Line %d - Number cannot end with . sign.",*lineNum);
                return NULL;
            }
            printf("Line %d - Number cannot end with E sign.",*lineNum);
            return NULL; //return error

        case EulNumExtra:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                curState = EulDouble;
                break;
            }
            printf("Line %d - Number cannot end with operator sign.",*lineNum);
            return NULL; //return error

        case EulDouble:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
            return tokenCtor(EuldDouble,*lineNum, "EuldDouble", data);
        //end of numbers

        case ExclamMark:
            if (curEdge == '=')
            {
                curState = ExclamEqual;break;
            }
            printf("Line %d - Lexical error.",*lineNum);
            return NULL; //return error

        case ExclamEqual:
            if (curEdge == '=')
                return tokenCtor(NotEqual,*lineNum, "NotEqual", data);
            printf("Line %d - Lexical error.",*lineNum);
            return NULL; //return error
            
        case GreaterThanSign:
            if (curEdge == '=')
                return tokenCtor(GreaterEqualThanSign,*lineNum, "GreaterEqualThanSign", data);
            fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
            return tokenCtor(GreaterThanSign,*lineNum, "GreaterThanSign", data);

        case LesserThanSign:
            if (curEdge == '=')
                return tokenCtor(LesserEqualThanSign,*lineNum, "LesserEqualThanSign", data);
            fseek(fp,-1,SEEK_CUR);//catch the "force-out" edge
            return tokenCtor(LesserThanSign,*lineNum, "LesserThanSign", data);

        case AlmostEndOfProgram:
            if (curEdge =! EOF)
                return NULL; //nothing else should follow this token - lexError            
            fseek(fp,-1,SEEK_CUR);//stay within the file
            return tokenCtor(EndOfProgram,*lineNum,"EndOfProgram",data);

        default:
            break;

        } // end of switch 

        curEdge = fgetc(fp); //get another edge
    }
    //DEBUG
    printf("unthinkable happend"); // should never happen
    return NULL;
}

void tokenDtor(Token *token)
{
    if (token!=NULL)
        free(token->data);
    free(token);
}

void listDtor(TokenList*list)
{
    for (int i = list->length; i >= 0; i--)
    {
        tokenDtor(list->TokenArray[i]);
    }
    free(list->TokenArray);
    free(list);
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
    
    TokenList *list; //structure to string tokens together
    Token* curToken; //token cursor

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
        if (curToken->type == Error)
        {
            free(curToken);
            break;
        }
        list = appendToken(list,curToken); //append to list
    }

    return list;
}

int main(int argc, char const *argv[])
{
    FILE *fp;
    // if (argc == 2)
    //     fp = fopen(argv[argc-1],"r");
    // else
    // {
    //     printf("ENTER FILE AS AN ARGUMENT");
    //     return 1;
    // }
        fp = fopen("../myTestFiles/test.txt","r");
    
    TokenList *list = lexAnalyser(fp);

    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return 1;
    }

    prinTokenList(list);

    listDtor(list);
    fclose(fp);

    return 0;
}
