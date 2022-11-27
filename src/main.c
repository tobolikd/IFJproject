#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "code-gen.h"
#include "error-codes.h"
#include "sem-analyzer.h"

ht_table_t *fncTable;

int main () {
    errorCode = SUCCESS;
    /* SCANNER */
    FILE *fp;

    fp = stdin;

    TokenList *list = lexAnalyser(fp); // get list of tokens
    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return errorCode;
    }

    fncTable = InitializedHTableFnctionDecs(list); //first descent
    if(fncTable == NULL) //error in first descent
    {
        debug_log("\n Error in first descent. Error code: %i.\n", errorCode);
        listDtor(list);
        fclose(fp);
        return errorCode;
    }

#if (DEBUG == 1)
    printTokenList(list);
#endif

    if(synAnalyser(list) == true)  // start syn analyzer
    {
        // .. go to code gen
    }
    
    // free memory
    listDtor(list);
    fclose(fp);

    debug_log("PROGRAM RETURNED %i.\n", errorCode);
    return errorCode;
}
