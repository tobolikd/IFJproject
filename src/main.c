#include "lex-analyzer.h"
#include "syn-analyzer.h"
#include "code-gen.h"
#include "error-codes.h"

int main () {
    /* SCANNER */
    FILE *fp;

    fp = stdin;

    TokenList *list = lexAnalyser(fp); // get list of tokens

    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return 1;
    }
#if (DEBUG == 1)
    prinTokenList(list);
#endif

    // free memory 
    listDtor(list);
    fclose(fp);
    /* END OF SCANNER */

    return 0;
}
