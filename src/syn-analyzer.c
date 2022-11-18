#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "symtable.h"
#include "error-codes.h"

#include <stdio.h>
#include <stdlib.h>

enum ifjErrCode errorCode;

void synAnalyser(TokenList *list)
{
    if (!list->TokenArray[0])
    {
        errorCode = SYNTAX_ERR;
        return;
    }
    return;
}
