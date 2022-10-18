#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lex-anal.h"

/* Global variable */
FILE *fp;


typedef struct 
{
    AutoState type; 
    char* lexeme;
    char* data; 
    int lineNum;
}Token;

typedef struct 
{
    Token *TokenArray;
    int length;
}TokenList;


/* TODO */
int checkProlog()
{
    return 0;
}

//reallocate memory for data
//return new pointer
void* attachData(char *data, char dataToBeInserted)
{
    int len;
    if (data == NULL)
        len = 2;
    else
        len = strlen(data)+2; // length of string + 1 + terminating null byte ('\0')
    
    data = realloc(data,(len)*sizeof(char));
    if (data == NULL)
        exit(111);
    
    data[len-2] = dataToBeInserted;
    data[len-1] = '\0';
    return data;
}


/* TODO */
/* needs to return a token!!! */
/* DELETE *curstate argument */
Token* foundToken(int lineNum, char* State, char* data)
{
    Token *new = malloc(sizeof(Token)); //allocates memory for new token
    new->data = data; //string already allocated
    new->lexeme = State;
    new->lineNum = lineNum;

    return new;
}

/* TODO RETURNS A TOKEN */
/* IT currently works with global variable of FILE *fp */

TokenList* insertToken(TokenList *list,Token* newToken)
{
    if(list == NULL)
    {
        list = malloc(sizeof(TokenList));
        list->TokenArray = malloc(2*sizeof(Token));
        list->length = 1;
    }
    else
    {
        list->length++;
        list->TokenArray = realloc(list->TokenArray,(list->length+1)*sizeof(Token));
    }
    // &list->TokenArray[list->length-1] = newToken;
    // &list->TokenArray[list->length] = NULL;
    return list;
}


//sends back pointer to a token with allocated memory and required data
Token* getToken(int *lineNum)
{
    int curState = Start;   
    char curEdge = fgetc(fp); //reads from file GLOBAL POINTER
    char *data=NULL;
    Token *token;
    while (1) //until you get a token -> whole finite state 
    {
        switch (curState)
        {
        case Start:
            if (curEdge == ' ') 
                break;

            if (curEdge == '\n')//count number of lines for function foundToken & debug
                *lineNum = *lineNum +1 ;
            
            if (curEdge == '/')
            {
                curState = Slash; break;
            }
            if (curEdge == '?')
            {
                curState = QuestionMark; break;
            }
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                curState = ID;
                // data = attachData(data, curEdge);//attach the caller
                fseek(fp,-1,SEEK_CUR);
                break;

            }
            if (curEdge >= '0' && curEdge <= '9')
            {
                curState = Int; 
                // data = attachData(data, curEdge); //attach the caller
                fseek(fp,-1,SEEK_CUR);
                break;
            }
            if (curEdge == '$')
            {
                curState = DollarSign; break;
            }
            if (curEdge == '"')
            {
                data = attachData(data, curEdge);
                curState = String; break;
            }
            if (curEdge == ';')
            {
                token = foundToken(*lineNum, "Semicolon", data);
                return token; break;
            }
            if (curEdge == '=')
            {
                token = foundToken(*lineNum, "Assign", data);
                return token; break;           
            }
            if (curEdge == ')')
            {
                token = foundToken(*lineNum, "RPar", data);
                return token; break;
            }
            if (curEdge == '(')
            {
                token = foundToken(*lineNum, "LPar", data);
                return token; break;
            }
            if (curEdge == '}')
            {
                token = foundToken(*lineNum, "RCurl", data);
                return token; break;
            }
            if (curEdge == '{')
            {
                token = foundToken(*lineNum, "LCurl", data);
                return token; break;
            }
            if (curEdge == EOF)
            {
                curState = Error;break;
            }
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
            break;
        case LineComment:
            if (curEdge == '\n')
            {
                curState = Start;
                break;
            }
            break;
        case StarComment:
            if (curEdge == '*')
            {
                curState = CommentStar;
            }
            break;
        case CommentStar:
            if (curEdge == '/')
            {
                curState = Start;
                break;
            }
            curState = StarComment;
            break;
        case QuestionMark:
            if (curEdge == '>')
            {
                curState = AlmostEndOfProgram; break;
            }
            else
            {
                token = foundToken(*lineNum, "QuestionMark", data);
                return token;          
            }
            break;
        case ID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                data = attachData(data, curEdge);
            }
            else
            {
                token = foundToken(*lineNum, "ID", data);
                fseek(fp,-1,SEEK_CUR);
                return token;
            }
            break;
        case DollarSign:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                curState = VarID;
                // data = attachData(data, curEdge); //attach the caller
                fseek(fp,-1,SEEK_CUR);
            }
            else
            {
                token = foundToken(*lineNum, "DollarSign", data);
                return token;
            }
            break;
            
        case VarID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
                data = attachData(data, curEdge);
            else
            {
                token = foundToken(*lineNum, "VarID", data);
                return token;
            }
            break;
        case String:
            if (curEdge == EOF)
            {
                curState = Error; break;
            }
            if (curEdge == '"')
            {
                data = attachData(data, curEdge);
                token = foundToken(*lineNum, "StringEnd", data);
                return token;
            }
            else
                data = attachData(data, curEdge);
            break;


        /* TODO numbers */
        case Int:
            token = foundToken(*lineNum, "Int", data);
            return token;

        case Error:
            return NULL;

        default:
            break;

        }

        if (curEdge == -1) //if EOF brake the loop
        {
            break;
        }
        
        curEdge = fgetc(fp); //get another edge
        
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    /* TODO - chceck arguments  */
    fp = fopen("../myTestFiles/test.txt" ,"r");  
    

    /* TODO check prolog*/
    if (checkProlog())
        return(1); //no prolog - lexical error 1 

    
    //loop through the program - get & send tokens
    int lineNum = 1;
    // Token* tokenArray = NULL;
    TokenList *list;

    while (1)
    {
        Token* curToken;
        curToken = getToken(&lineNum);

        // list = insertToken(list,curToken);

        //if getToken returned NULL - error ! 
        if (curToken == NULL)
            break;

        //insert token to an array 
        // insertToken();
        if (curToken->data == NULL)
            // printf("%s %d\n",list->TokenArray[list->length-1].lexeme,list->TokenArray[list->length-1].lineNum);
            printf("%s %d\n",curToken->lexeme,curToken->lineNum);
        else
            printf("%s %d %s\n",curToken->lexeme,curToken->lineNum,curToken->data);
            // printf("%s %d %s\n",list->TokenArray[list->length-1].lexeme,list->TokenArray[list->length-1].lineNum, list->TokenArray[list->length-1].data);
        free(curToken->data);
        free(curToken);

    }
    
    /* send an array of tokens */


}
