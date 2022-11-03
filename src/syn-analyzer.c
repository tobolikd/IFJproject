#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "error-codes.h"

bool isIntegrated(char *functionId)
{
    const char* integratedFunctions[] = { "reads", "readi", "readf", "write", "floatval", "intval", "strval", "strlen", "substring", "ord", "chr" };

    for (long unsigned int i = 0; i < sizeof(integratedFunctions) / sizeof(char *); i++)
    {
        if (!strcmp(functionId, integratedFunctions[i]))
            return true;
    }
    return false;
}
