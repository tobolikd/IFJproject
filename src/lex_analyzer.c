/**
 * IFJ22 compiler
 *
 * @file lex_alyzer.c
 * 
 * @brief Scanner module. 
 * @brief Module to read and sort input file into tokens, which allows
 *        other modules to work efficiently.
 *
 * @author Gabriel Biel(xbielg00)
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lex_analyzer.h"
#include "error_codes.h"

// init STATE_STRING
const char *STATE_STRING[] =
{
    FOREACH_STATE(GENERATE_STRING)
};

// init TOKEN_TYPE_STRING
const char *TOKEN_TYPE_STRING[] =
{
    FOREACH_TOKEN_TYPE(GENERATE_STRING)
};

//check if prolog is present
int checkProlog(FILE* fp, int *lineNum)
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
                            //white sign, comment..
                            if (c=='\n' || c ==EOF || c == ' ' || c == '\t' || c == '/'|| c == '\r')
                                //check declare type ..
                                if(!checkDeclare(fp,lineNum))
                                    return 0;
                        }
    THROW_ERROR(LEXICAL_ERR,1);
    debug_print("%s : Mistake in prolog.\n",prolog);
    return 1;
}

int checkDeclare(FILE *fp,int *lineNum)
{
    TokenList *tmpList = NULL;

    //arrays to compare with
    char *cmpData[7] = {"declare",NULL,"strict_types",NULL,"1",NULL,NULL};
    TokenType cmpType[7] = {t_functionId,t_lPar,t_functionId,t_assign,t_int,t_rPar,t_semicolon};

    for (int i = 0; i < 7; i++) //get all tokens declare ( strict_types = 1 ) ; = 7 in total
    {       
        tmpList = appendToken(tmpList,getToken(fp,lineNum)); //append next token to list of token

        if(tmpList->TokenArray[i] == NULL)//expected 7 tokens, not less
        {
            THROW_ERROR(LEXICAL_ERR,1);
            free(tmpList);
            return 1;
        }

        if (tmpList->TokenArray[i]->type == cmpType[i])//compare token type
        {
            if(cmpType[i] == t_functionId || cmpType[i] == t_int)//if id / int
            {    // if those are the same go onto another token
                if (!strcmp(tmpList->TokenArray[i]->data , cmpData[i]))
                    continue; //data are valid
            }
            else
                continue; //type is valid, where data is irelevant
        }
        listDtor(tmpList); //either type or data is not right
        debug_print("%d - Mistake in declare - types\n",i);
        return 1;
    }
    listDtor(tmpList);
    return 0;
}


Token *getKeyword(Token *token)
{
    if (token->data!=NULL)
    {
        if (!strcmp(token->data,"if"))
        {
            token->type = t_if;
            free(token->data);
            token->data=NULL;
        }
        else if (!strcmp(token->data,"while"))
        {
            token->type = t_while;
            free(token->data);
            token->data=NULL;
        }
        else if (!strcmp(token->data,"int")||!strcmp(token->data,"string")||!strcmp(token->data,"float")||!strcmp(token->data,"void"))
        {
            token->type = t_type;
        }
        else if (!strcmp(token->data,"else"))
        {
            token->type = t_else;
            free(token->data);
            token->data=NULL;
        }
        else if (!strcmp(token->data,"return"))
        {
            token->type = t_return;
            free(token->data);
            token->data=NULL;

        }
        else if (!strcmp(token->data,"null"))
        {
            token->type = t_null;
            free(token->data);
            token->data=NULL;
        }
        else if (!strcmp(token->data,"function"))
        {
            token->type = t_function;
            free(token->data);
            token->data=NULL;
        }
    }
    return token;
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
    CHECK_MALLOC_PTR(data);

    data[len-1] = dataToBeInserted;
    data[len] = '\0';
    return data;
}

char *parseString(char *data)
{
    int i = 0;
    char *new = NULL;

    if (data[i] == '\0')
        return data;


    while (data[i] != '\0') //look through the whole data string
    {
        if (data[i] == '\\') //if bs appears go through valid sequenses
        {
            i++;//look 1 ahead.
            switch (data[i])
            {
            case '\\':
                new = appendChar(new, data[i]);
                break;
            case 'n':
                data[i] = '\n';
                new = appendChar(new, data[i]);
                break;
            case 't':
                data[i] = '\t';
                new = appendChar(new, data[i]);
                break;
            case '$':
                new = appendChar(new, data[i]);
                break;

            case 'x'://look for haxa num
                i++;//look 1 ahead = now i is pointing to first digit
                //strtol functioni returns value of number in string, stops only when the sequence of numbers is broken
                //could possibly read more or less than 2 digits
                //works on base of 10/8/16 -> transfers it to decimal
                //could move a pointer with 2nd argument but it is not wanted
                if ( 256 > strtol(data+i,NULL,16) && strtol(data+i,NULL,16) > 0 && isxdigit(data[i+1])) // check if there are 2 digits
                {
                    new = appendChar(new,(char)strtol(data+i,NULL,16) );//strtol converts -> append value to new arr
                    i +=1;//move pointer
                    break;
                }
                free(data);
                free(new);
                return NULL; // error

            case '0' ... '9'://look for okta numbers
                if(isdigit(data[i+1]) && isdigit(data[i+2])) // 2 hexa digits must following
                {
                    //from characters value subs value ord value of 0 and we get an integer value representing the number in char
                    if (((int)data[i]-'0')<=3 && ((int)data[i+1]-'0')<=7 && ((int)data[i+2]-'0')<=7)//check if its okta number
                    {
                        //temporary array to only take into accout first 3 digits so strol(conversion) can be performed on only firs 3 digits
                        char tmp[4] = {data[i], data[i+1], data[i+2], '\0'};

                        if ( 256 > (int)strtol(tmp,NULL,8) && (int)strtol(tmp,NULL,8) > 0) //check value
                        {
                            new = appendChar(new, (char)strtol(tmp,NULL,8));//strtol converts
                            i +=2;//move poiter
                            break;
                        }
                    }
                }
                free(data);
                free(new);
                return NULL; // error

            default: //not an escape sequence put bs back into the array
                i--;//needs to go 1 back
                new = appendChar(new,'\\');

            }//end of switch
        }
        else //if there cur is not backslash
        {
            if (data[i] == '$') //dollar may only be inserted after backslash
            {
                free(data);
                free(new);
                return NULL; // error
            }
            new = appendChar(new, data[i]);
        }

        i++;//move onto next character

    }   //while cycle
    free(data);
    return new;
}

Token* tokenCtor(TokenType type, int lineNum, char* data)
{
    Token *new = malloc(sizeof(Token)); //allocates memory for new token
    CHECK_MALLOC_PTR(new);
    new->data = data; //string already allocated
    new->lineNum = lineNum;
    new->type = type;

    if (type == t_functionId)
        new = getKeyword(new);

    if (type == t_string)
    {
        if (new->data != NULL)
        {
            new->data = parseString(new->data);
            if (new->data == NULL)
            {
                debug_log("Lexical error inside string. fnc parseString");
                THROW_ERROR(LEXICAL_ERR,lineNum);
                tokenDtor(new);
                return NULL; // error inside string
            }
        }
    }

    return new;
}

//appends token to list of tokens
TokenList *appendToken(TokenList *list, Token* newToken)
{
    //new tokenList - initiate
    if(list == NULL)
    {
        list = malloc(sizeof(TokenList));
        CHECK_MALLOC_PTR(list);
        list->TokenArray = malloc(2*sizeof(Token));
        list->length = 1;
    }
    else
    {
        //allocate memory for new token
        list->length++;
        list->TokenArray = realloc(list->TokenArray,(list->length+1)*sizeof(Token));
    }
    CHECK_MALLOC_PTR(list->TokenArray);

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
                return tokenCtor(t_EOF, *lineNum, data);

            case String:
                free(data);
                return NULL;

            case ID:
                return tokenCtor(t_functionId, *lineNum, data);

            case StarComment:
                return NULL;

            case CommentStar:
                return NULL;

            case LineComment:
                return tokenCtor(t_EOF, *lineNum, data);

            case AlmostEndOfProgram:
                return tokenCtor(t_EOF, *lineNum, data);

            case QuestionMark:
                if (checkDataType(data))
                    return tokenCtor(t_nullType, *lineNum, data);
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
            if (curEdge == ' ' || curEdge == '\t' || curEdge == '\r')
                break;
            if (curEdge == '\n' )//count number of lines for function foundToken & debug
            {
                *lineNum = *lineNum +1;
                break;
            }
            if (curEdge == '/')
            {
                data = appendChar(data, curEdge);
                curState = Slash; break;
            }
            if (curEdge == '?')
            {
                curState = QuestionMark; break;
            }
            if (isalpha(curEdge)|| curEdge == '_')
            {
                data = appendChar(data, curEdge);//attach the caller
                curState = ID;
                break;
            }
            if (isdigit(curEdge))
            {
                data = appendChar(data, curEdge); //attach the caller
                curState = Int;
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
                data = appendChar(data, curEdge); //attach the caller
                break;
            }
            if (curEdge =='<')
            {
                data = appendChar(data, curEdge); //attach the caller
                curState = GreaterThanSign; break;
            }
            if (curEdge =='>')
            {
                data = appendChar(data, curEdge); //attach the caller
                curState = LesserThanSign; break;
            }
            if (curEdge =='!')
            {
                data = appendChar(data, curEdge); //attach the caller
                curState = ExclamMark;
                break;
            }
            if (curEdge == ';')
            	return tokenCtor(t_semicolon, *lineNum, data);
            if (curEdge == ')')
                return tokenCtor(t_rPar, *lineNum, data);
            if (curEdge == '(')
            	return tokenCtor(t_lPar, *lineNum, data);
            if (curEdge == '}')
                return tokenCtor(t_rCurl, *lineNum, data);
            if (curEdge == '{')
            	return tokenCtor(t_lCurl, *lineNum, data);
            if (curEdge =='*')
            {
            	data = appendChar(data, curEdge);
            	return tokenCtor(t_operator, *lineNum, data);
            }
            if (curEdge =='-')
            {
            	data = appendChar(data, curEdge);
            	return tokenCtor(t_operator, *lineNum, data);
            }
            if (curEdge =='+')
            {
            	data = appendChar(data, curEdge);
            	return tokenCtor(t_operator, *lineNum, data);
            }
            if (curEdge =='.')
            {
            	data = appendChar(data, curEdge);
            	return tokenCtor(t_operator, *lineNum, data);
            }
            if (curEdge ==',')
            	return tokenCtor(t_comma, *lineNum, data);
            if (curEdge ==':')
            	return tokenCtor(t_colon, *lineNum, data);

            debug_print("At line: %d START -> %c : Not recognised\n", *lineNum,curEdge);
            free(data);
            return NULL;
            break;

        case Slash:
            if (curEdge == '/')
            {
                free(data);
                data = NULL;
                curState = LineComment; break;
            }
            if (curEdge == '*')
            {
                free(data);
                data = NULL;
                curState = StarComment; break;
            }
            ungetc(curEdge,fp); //catch the "force-out" edge
            return tokenCtor(t_operator, *lineNum, data);

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
            if (isalpha(curEdge))//look for data type
            {
                data = appendChar(data, curEdge);
                break;
            }
            else if (curEdge == '>')
            {
                if (data != NULL) // > came after some characters
                {
                    free(data);
                    return NULL;
                }
                curState = AlmostEndOfProgram;
                break;
            }
            if (checkDataType(data))//returns 1 it is a valid data type
            {
                ungetc(curEdge,fp); //catch the "force-out" edge
                return tokenCtor(t_nullType, *lineNum, data);
            }
            free(data);
            return NULL;

        case AlmostEndOfProgram:
            free(data);
            return NULL; //nothing should come after ?>

        case ID:
            if (isalnum(curEdge) || curEdge == '_')  //alphanumeric or punctuation mark
                data = appendChar(data, curEdge);
            else
            {
                ungetc(curEdge,fp);
                return tokenCtor(t_functionId, *lineNum, data);
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
            if (isalnum(curEdge) || curEdge == '_')
            {
                data = appendChar(data, curEdge);
                break;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_varId, *lineNum, data);

        case String:
            if (curEdge == '"') //if string should end
            {
                if (data != NULL)
                {
                    int cursor = strlen(data); //loop through the array
                    int count = 0; //number of \\bs

                    while (cursor > 0) // solve repeating \\bs
                    {
                        if(data[cursor-1] != '\\') // in case there was \\bs before
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
                else
                    data = appendChar(data, '\0'); //append empty string

                return tokenCtor(t_string, *lineNum, data);
            }
            else if (isprint(curEdge)) //if printable -> append
            {
                data = appendChar(data, curEdge);
                break;
            }
            //weird data on the input
            debug_print("At line: %d STRING -> %c : Not recognised\n", *lineNum,curEdge);

            return NULL;

        case Assign:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                curState = DoubleEqual; break;
            }
            ungetc(curEdge, fp);
            return tokenCtor(t_assign, *lineNum, data);

        case DoubleEqual:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                return tokenCtor(t_comparator, *lineNum, data);
            }
            THROW_ERROR(LEXICAL_ERR,*lineNum);
            free(data);
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
                curState = ExpNum;
                break;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_int, *lineNum, data);

        case Double:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            if (curEdge == 'e' || curEdge == 'E')
            {
                data = appendChar(data,curEdge);
                curState = ExpNum;
                break;
            }
            //if there is anything else...
            //but if there is nothing after dot means error
            if (data[strlen(data)-1] == '.')
            {
                debug_print("Line %d - Number cannot end with . sign. \n", *lineNum);
                free(data);
                return NULL;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_float, *lineNum, data);

        case ExpNum:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                curState = ExpDouble;
                break;
            }
            if (curEdge == '+' || curEdge == '-')
            {
                data = appendChar(data,curEdge);
                curState = ExpNumExtra;
                break;
            }
            //cannot end with e as its last character
            debug_print("Line %d - Number cannot end with e sign. \n", *lineNum);
            free(data);
            return NULL;

        case ExpNumExtra:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                curState = ExpDouble;
                break;
            }
            debug_print("Number cannot end with operator sign.\n");
            THROW_ERROR(LEXICAL_ERR,*lineNum);
            free(data);
            return NULL; //return error

        case ExpDouble:
            if (isdigit(curEdge))
            {
                data = appendChar(data,curEdge);
                break;
            }
            //previous state == ExpNum
            if (data[strlen(data)-1] == 'e' || data[strlen(data)-1] == 'E' )
            {
                debug_print("Line %d - %c Unpropper double number ending.\n", *lineNum,curEdge);
                free(data);
                return NULL;
            }
            //previous state == ExpNUmExtra
            if (data[strlen(data)-1] == '+' || data[strlen(data)-1] == '-' )
            {
                debug_print("Line %d - %c Unpropper double number ending.\n", *lineNum,curEdge);
                free(data);
                return NULL;
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_float, *lineNum, data);
        //end of numbers

        case ExclamMark:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                curState = ExclamEqual;break;
            }
            THROW_ERROR(LEXICAL_ERR,*lineNum);
            free(data);
            return NULL; //return error

        case ExclamEqual:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                return tokenCtor(t_comparator, *lineNum, data);
            }
            THROW_ERROR(LEXICAL_ERR,*lineNum);
            free(data);
            return NULL; //return error

        case GreaterThanSign:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                return tokenCtor(t_comparator, *lineNum, data);
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_comparator, *lineNum, data);

        case LesserThanSign:
            if (curEdge == '=')
            {
                data = appendChar(data, curEdge);
                return tokenCtor(t_comparator, *lineNum, data);
            }
            ungetc(curEdge,fp);//catch the "force-out" edge
            return tokenCtor(t_comparator, *lineNum, data);

        default:
            break;

        } // end of switch

        if(curEdge == EOF && curState != Start)
        {
            free(data);
            return NULL;
        }
        curEdge = fgetc(fp); //get another edge
    }
    debug_log("unthinkable happend\n"); // should never happen
    free(data);
    return NULL;
}

void tokenDtor(Token *token)
{
    if (token != NULL)
    {
        if (token->data != NULL)
            free(token->data);

        free(token);
    }
}

void listDtor(TokenList *list)
{
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
}

void printToken(Token*token)
{
    if (token->data == NULL){
        debug_log("%d %s\n", token->lineNum, TOKEN_TYPE_STRING[token->type]);
    }
    else{
        debug_log("%d %s %s\n", token->lineNum, TOKEN_TYPE_STRING[token->type], token->data);
    }
}


void printTokenList(TokenList *list)
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
    int lineNum = 1; //lin number

    if (checkProlog(fp,&lineNum))
        return NULL;

    TokenList *list = NULL; //structure to string tokens together
    Token* curToken = NULL; //token cursor

    //loop through the program - get & append tokens
    while (1)
    {
        curToken = getToken(fp,&lineNum); //get token

        //getToken returned NULL this means error !
        if (curToken == NULL)
        {
            listDtor(list);
            THROW_ERROR(LEX_ANALYZER_H,lineNum);
            return NULL;
        }

        list = appendToken(list,curToken); //append to list

        if (curToken->type == t_EOF)
            break;
    }

    return list;
}
