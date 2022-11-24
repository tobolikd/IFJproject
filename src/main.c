#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "code-gen.h"
#include "error-codes.h"
#include "sem-analyzer.h"

int main () {
    errorCode = SUCCESS;
    /* SCANNER */
    FILE *fp;

    fp = stdin;

    TokenList *list = lexAnalyser(fp); // get list of tokens
    ht_table_t *table = InitializedHTableFnctionDecs(list);


    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return errorCode;
    }
    if(table == NULL){ //error in hashtable
        debug_log("\n %i \n", errorCode);
        listDtor(list);
        fclose(fp);
        return errorCode;
    }
#if (DEBUG == 1)
    //printTokenList(list);
#endif
    //synAnalyser(list);  // start syn analyzer

    // free memory
    listDtor(list);
    fclose(fp);
    /* END OF SCANNER */
    debug_log("\n %i \n", errorCode);
    return errorCode;
}
