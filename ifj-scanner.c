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
    AutoState State;
    char* type;
}Lexeme;




/* TODO */
int checkProlog()
{
    return 0;
}
/* TODO */
/* move cursor 1 back */
/* set curState na start */
int gotToken(int *curState, AutoState State)
{
    *curState = Start;
    printf("%d\n",State);
    return 1;
}

int getNextToken()
{
    int curState = Start;
    char curEdge = fgetc(fp);

    while (1)
    {
        switch (curState)
        {
        case Start:
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
                curState = ID; break;
            }
            if (curEdge >= '0' && curEdge <= '9')
            {
                curState = Int; break;
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
                gotToken(&curState, Semicolon); break;
            }
            if (curEdge == '=')
            {
                gotToken(&curState, Assign); break;            }
            if (curEdge == ')')
            {
                gotToken(&curState, RPar); break;
            }
            if (curEdge == '(')
            {
                gotToken(&curState, LPar); break;
            }
            if (curEdge == '}')
            {
                gotToken(&curState, RCurl); break;
            }
            if (curEdge == '{')
            {
                gotToken(&curState, LCurl); break;
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
        /* comments */
        case LineComment:
            if (curEdge == '\n')
            {
                gotToken(&curState, CommentEnd);
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
                gotToken(&curState, CommentEnd);
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
                gotToken(&curState, QuestionMark);          
            }
            break;
        case ID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                attachData();
            }
            else
            {
                gotToken(&curState, ID);
            }
            break;
        case DollarSign:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                curState = VarID;
                attachData();
            }
            else
            {
                gotToken(&curState, DollarSign);
            }
            break;
            
        case VarID:
            if ((curEdge >= 'A' && curEdge <= 'Z') || (curEdge >= 'a' && curEdge <= 'z'))
            {
                attachData();
            }
            else
            {
                gotToken(&curState, VarID);
            }
            break;
        case String:
            if (curEdge == EOF)
            {
                curState = Error; break;
            }
            if (curEdge == '"')
            {
                gotToken(&curState, StringEnd);
            }
            else
            {
                attachData();
            }
            break;


        /* TODO numbers */
        case Int:
            gotToken(&curState, Int); break;
            break;

        default:
            break;
        }


        curEdge = fgetc(fp);
        if (curEdge == -1)
        {
            break;
        }
        
    }
    return 0;
}

void attachData()
{
    return ;
}

int main(int argc, char const *argv[])
{
    /* TODO - chceck arguments  */
    fp = fopen("test.txt" ,"r");  
    

    /* TODO */
    if (checkProlog())
        exit(1); //no prolog - lexical error 1 

    
    //loop through the program - get & send tokens
    while (1)
    {
        getNextToken();

        attachData();

        printf("TOKEN\n");

        if (0) //EOF
            break;

        break;    
    }
    
}
