#include "lex-anal.h"
#include "syn-anal.h"
#include "code-gen.h"

int main (int argc,char**argv) {
    /* SCANNER */
    FILE *fp;

    if (argc == 2)
        fp = fopen(argv[argc-1],"r");
    else
    {
        printf("ENTER A FILE AS ARGUMENT.\n");
        return 1;
    }

    TokenList *list = lexAnalyser(fp); // get list of tokens

    if(list == NULL) // there was an error in lexAnalyser
    {
        fclose(fp);
        return 1;
    }
    //DEBUG
    prinTokenList(list);

    // free memory 
    listDtor(list);
    fclose(fp);
    /* END OF SCANNER */

    return 0;
}
