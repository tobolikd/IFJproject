#include "lex-analyzer.h"
#include "sem-analyzer.h"
#include "error-codes.h"
#include "symtable.h"

#include <stdbool.h>
#include <string.h>

const char *EXPR_TYPE_STRING[] =
{
    FOREACH_EXPR_TYPE(GENERATE_STRING)
};

ht_table_t *initFncSymtable()
{
    ht_table_t fncSymtable;
    ht_init(fncSymtable);
    /*add each integrated function*/
    // "reads"
    // "readi"
    // "readf"
    // "write"
    // "floatval"
    // "intval"
    // "strval"
    // "strlen"
    // "substring"
    // "ord"
    // "chr"
}
