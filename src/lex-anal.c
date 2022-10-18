#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lex-anal.h"

/* Global variable */
FILE *fp;

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

typedef struct 
{
    AutoState type; 
    char* lexeme;
    char* data; 
    int lineNum;
}Token;


/* TODO */
int checkProlog()
{
    return 0;
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
Token* getToken()
{
    int curState = Start;   
    char curEdge = fgetc(fp); 
    char *data=NULL;
    int lineNum =1; //drop 1 level

    /* TODO break the loop & send token to main */
    while (1) //until you get a token -> whole finite state 
    {
        switch (curState)
        {
        case Start:
            if (curEdge == ' ')
                break;

            if (curEdge == '\n')//count number of lines for function foundToken & debug
                lineNum++;
            
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
                curState = String; break;
            }
            if (curEdge == ';')
            {
                return foundToken(lineNum, "Semicolon", NULL); break;
            }
            if (curEdge == '=')
            {
                return foundToken(lineNum, "Assign", NULL); break;            }
            if (curEdge == ')')
            {
                return foundToken(lineNum, "RPar", NULL); break;
            }
            if (curEdge == '(')
            {
                return foundToken(lineNum, "LPar", NULL); break;
            }
            if (curEdge == '}')
            {
                return foundToken(lineNum, "RCurl", NULL); break;
            }
            if (curEdge == '{')
            {
                return foundToken(lineNum, "LCurl", NULL); break;
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
                return foundToken(lineNum, "QuestionMark", NULL);          
            }
            break;
        case ID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                data = attachData(data, curEdge);
            }
            else
            {
                return foundToken(lineNum, "ID", NULL);
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
                return foundToken(lineNum, "DollarSign", NULL);
            }
            break;
            
        case VarID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                data = attachData(data, curEdge);
            }
            else
            {
                return foundToken(lineNum, "VarID", NULL);
            }
            break;
        case String:
            if (curEdge == EOF)
            {
                curState = Error; break;
            }
            if (curEdge == '"')
            {
                return foundToken(lineNum, "StringEnd", NULL);
            }
            else
            {
                data = attachData(data, curEdge);
            }
            break;


        /* TODO numbers */
        case Int:
            return foundToken(lineNum, "Int", NULL); break;

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

//reallocate memory for data
//return new pointer
void* attachData(char *data, char dataToBeInserted)
{
    int len = strlen(data)+1; // length of string + terminating null byte ('\0')
    if (data == NULL)
        len = 1;
    
    data = realloc(data,(len+1)*sizeof(char));
    if (data == NULL)
        exit(111);
    
    data[-2] = dataToBeInserted;
    return data;
}

int main(int argc, char const *argv[])
{
    /* TODO - chceck arguments  */
    fp = fopen("../myTestFiles/test.txt" ,"r");  
    

    /* TODO check prolog*/
    if (checkProlog())
        return(1); //no prolog - lexical error 1 

    
    //loop through the program - get & send tokens
    while (1)
    {
        Token* curToken;
        curToken = getToken();

        //if token.type == error => lexical error 1
        if (curToken->type == Error)
            return (1);

        //insert token to an array 
        // insertToken();

        printf("%s %d\n",curToken->lexeme,curToken->lineNum);

        free(curToken->data);
        free(curToken);

        break;    
    }
    
    /* send an array of tokens */


}
