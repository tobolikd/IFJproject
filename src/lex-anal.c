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
    Token **TokenArray;
    int length;
}TokenList;

//check if prolog is present
void checkProlog()
{
    char *prolog = "<?php";
    if(prolog[0]==fgetc(fp) )
        if(prolog[1]==fgetc(fp))
            if(prolog[2]==fgetc(fp))
                if(prolog[3]==fgetc(fp))  
                    if(prolog[4]==fgetc(fp))
                        return;
    printf("%s : prolog is missing",prolog);
    exit(1);                   
}

//allocate memory for data
//return new pointer
void* attachData(char *data, char dataToBeInserted)
{
    int len;
    if (data == NULL)
        len = 1;
    else
        len = strlen(data)+1; // length of string + 1 + terminating null byte ('\0')
    
    data = realloc(data,(len+1)*sizeof(char));
    if (data == NULL)
        exit(111);
    
    data[len-1] = dataToBeInserted;
    data[len] = '\0';
    return data;
}

//setup token
//return new token
Token* tokenCtor(int lineNum, char* State, char* data)
{
    Token *new = malloc(sizeof(Token)); //allocates memory for new token
    new->data = data; //string already allocated
    new->lexeme = State;
    new->lineNum = lineNum;
    return new;
}

//appends token to list of tokens
void *appendToken(TokenList *list, Token* newToken)
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
    list->TokenArray[list->length-1] = newToken;
    list->TokenArray[list->length] = NULL;
    return list;
}

/* TODO error stavy */
/* TODO dokoncit stavy z KA mat. operatory, cisla atd.. */
/* TODO EOF edge v jednolivych stavoch*/
//returns pointer to setup token
//returns NULL if error accured
Token* getToken(int *lineNum)
{
    int curState = Start;
    char curEdge = fgetc(fp); //reads from file GLOBAL POINTER
    char *data=NULL;

    while (1) //until you get a token -> whole finite state 
    {

        //to break the loop 
        if (curEdge == EOF)
            curState = Error;
        

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
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                curState = ID;
                // data = attachData(data, curEdge);//attach the caller
                fseek(fp,-1,SEEK_CUR); break;
            }
            if (curEdge >= '0' && curEdge <= '9')
            {
                curState = Int; 
                // data = attachData(data, curEdge); //attach the caller
                fseek(fp,-1,SEEK_CUR); break;
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
                return tokenCtor(*lineNum, "Semicolon", data);
            if (curEdge == '=')
                return tokenCtor(*lineNum, "Assign", data);
            if (curEdge == ')')
                return tokenCtor(*lineNum, "RPar", data);
            if (curEdge == '(')
                return tokenCtor(*lineNum, "LPar", data);
            if (curEdge == '}')
                return tokenCtor(*lineNum, "RCurl", data);
            if (curEdge == '{')
                return tokenCtor(*lineNum, "LCurl", data);
            
            //unknow ATM
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
                return tokenCtor(*lineNum, "QuestionMark", data);
            }
            break;
        case ID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                data = attachData(data, curEdge);
            }
            else
            {
                fseek(fp,-1,SEEK_CUR);
                return tokenCtor(*lineNum, "ID", data);
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
                return tokenCtor(*lineNum, "DollarSign", data);
            }
            break;
            
        case VarID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
                data = attachData(data, curEdge);
            else
            {
                return tokenCtor(*lineNum, "VarID", data);
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
                return tokenCtor(*lineNum, "StringEnd", data);
            }
            else
                data = attachData(data, curEdge);
            break;

        /* TODO numbers */
        case Int:
            return tokenCtor(*lineNum, "Int", data);

        case Error:
            return NULL;

        default:
            break;

        }

        curEdge = fgetc(fp); //get another edge
    }
    printf("unthinkable happend"); // should never happen
    return NULL;
}

int main(int argc, char const *argv[])
{
    /* TODO - chceck arguments for file */
    fp = fopen("../myTestFiles/test.txt" ,"r");  
    
    checkProlog();

    
    TokenList *list; //structure to string tokens together
    Token* curToken; //token cursor

    int lineNum = 1; //on what line token is found

    //loop through the program - get & send tokens
    while (1)
    {

        curToken = getToken(&lineNum);

        list = appendToken(list,curToken);

        //if getToken returned NULL - error ! 
        if (curToken == NULL)
            break;

        if (curToken->data == NULL)
            printf("%s %d\n",list->TokenArray[list->length-1]->lexeme,list->TokenArray[list->length-1]->lineNum);
            // printf("%s %d\n",curToken->lexeme,curToken->lineNum);
        else
            // printf("%s %d %s\n",curToken->lexeme,curToken->lineNum,curToken->data);
            printf("%s %d %s\n",list->TokenArray[list->length-1]->lexeme,list->TokenArray[list->length-1]->lineNum, list->TokenArray[list->length-1]->data);
        

        /* TODO tokenDtor */
        free(curToken->data);
        free(curToken);

    }
    /* todo listDtor */
    free(list->TokenArray);
    free(list);
    
}
