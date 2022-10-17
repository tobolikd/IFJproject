#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define ASCII 128

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
int gotToken(int lineNum, int *curState, char* State, char* data)
{
    *curState = Start;
    printf("%s %d\n",State,lineNum);
    return 1;
}

int getNextToken()
{
    int curState = Start;   
    char curEdge = fgetc(fp); 
    int lineNum =1; //drop 1 level
    /* TODO redo for Token structure */
    Token *token; 
    /* TODO break the loop & send token to main */
    while (1) //until you get a token 
    {
        switch (curState)
        {
        case Start:
            if (curEdge == ' ')
                break;

            if (curEdge == '\n')//count number of lines for function gotToken & debug
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
                fseek(fp,-1,SEEK_CUR); //catch the first letter
                break;

            }
            if (curEdge >= '0' && curEdge <= '9')
            {
                curState = Int; 
                fseek(fp,-1,SEEK_CUR); //catch the first number
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
                gotToken(lineNum, &curState, "Semicolon", NULL); break;
            }
            if (curEdge == '=')
            {
                gotToken(lineNum, &curState, "Assign", NULL); break;            }
            if (curEdge == ')')
            {
                gotToken(lineNum, &curState, "RPar", NULL); break;
            }
            if (curEdge == '(')
            {
                gotToken(lineNum, &curState, "LPar", NULL); break;
            }
            if (curEdge == '}')
            {
                gotToken(lineNum, &curState, "RCurl", NULL); break;
            }
            if (curEdge == '{')
            {
                gotToken(lineNum, &curState, "LCurl", NULL); break;
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
                gotToken(lineNum, &curState, "QuestionMark", NULL);          
            }
            break;
        case ID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                attachData();
            }
            else
            {
                gotToken(lineNum, &curState, "ID", NULL);
            }
            break;
        case DollarSign:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                curState = VarID;
                fseek(fp,-1,SEEK_CUR);//catch the caller
            }
            else
            {
                gotToken(lineNum, &curState, "DollarSign", NULL);
            }
            break;
            
        case VarID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                attachData();
            }
            else
            {
                gotToken(lineNum, &curState, "VarID", NULL);
            }
            break;
        case String:
            if (curEdge == EOF)
            {
                curState = Error; break;
            }
            if (curEdge == '"')
            {
                gotToken(lineNum, &curState, "StringEnd", NULL);
            }
            else
            {
                attachData();
            }
            break;


        /* TODO numbers */
        case Int:
            gotToken(lineNum, &curState, "Int", NULL); break;

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
void* attachData(/* pointer to data string */)
{
    return NULL;
}

int main(int argc, char const *argv[])
{
    /* TODO - chceck arguments  */
    fp = fopen("../myTestFiles/test.txt" ,"r");  
    

    /* TODO check prolog*/
    if (checkProlog())
        exit(1); //no prolog - lexical error 1 

    
    //loop through the program - get & send tokens
    while (1)
    {
        getNextToken();

        if (0) //EOF
            break;

        break;    
    }
    
}
