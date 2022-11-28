#include "code-gen-data.h"
#include "code-gen.h"
#include "symtable.h"

#include <stdio.h>

void genBuiltIns(ht_table_t *varSymtable) {
    printf(".IFJcode22\n");

    // generate global vars
    //
    // black hole for disposing values and tmp use
    INST_DEFVAR(VAR_CODE("GF", "black%hole"));
    // aux var for resolve%condition
    INST_DEFVAR(VAR_CODE("GF", "aux%condition"));

    // jump over pregenerated code
    INST_JUMP(LABEL("end%pregenerated%end"));

    // generate functions (built in and aux)

    genResolveCondition();
    genExitLabels();

    // end pregenerated
    INST_LABEL(LABEL("end%pregenerated%end"));

    COMMENT("END OF PREGENERATED PART");
    COMMENT("BEGINING OF PROGRAM PART");
}

void genResolveCondition() {
    INST_LABEL(LABEL("resolve%condition"));

	INST_POPS(VAR_CODE("GF", "aux%condition"));
	INST_PUSHS(VAR_CODE("GF", "aux%condition"));// save back to stack
	INST_TYPE(VAR_CODE("GF", "aux%condition"), VAR_CODE("GF", "aux%condition")); // save type

	INST_JUMPIFEQ(LABEL("cond%int"), CONST_STRING("int"), VAR_CODE("GF", "aux%condition"));
	INST_JUMPIFEQ(LABEL("cond%bool"), CONST_STRING("bool"), VAR_CODE("GF", "aux%condition"));
	INST_JUMPIFEQ(LABEL("cond%float"), CONST_STRING("float"), VAR_CODE("GF", "aux%condition"));
	INST_JUMPIFEQ(LABEL("cond%string"), CONST_STRING("string"), VAR_CODE("GF", "aux%condition"));
	INST_JUMPIFEQ(LABEL("cond%nil"), CONST_STRING("nil"), VAR_CODE("GF", "aux%condition"));

	INST_EXIT(CONST_INT(99)); // variable not initialized

	// true
	INST_LABEL(LABEL("cond%true"));
	INST_PUSHS(CONST_BOOL("true"));
    INST_RETURN();

	// false
	INST_LABEL(LABEL("cond%false"));
	INST_PUSHS(CONST_BOOL("false"));
    INST_RETURN();

	//int
	INST_LABEL(LABEL("cond%int"));
	INST_PUSHS(CONST_INT(0));
	INST_JUMPIFEQS(LABEL("cond%false"));
	INST_JUMP(LABEL("cond%true"));

	// bool - value already on top of stack
	INST_LABEL(LABEL("cond%bool"));
    INST_RETURN();

	// float
	INST_LABEL(LABEL("cond%float"));
	INST_PUSHS(CONST_FLOAT((double) 0));
	INST_JUMPIFEQS(LABEL("cond%false"));
	INST_JUMP(LABEL("cond%true"));

	// string
	INST_LABEL(LABEL("cond%string"));
	INST_PUSHS(CONST_STRING("0"));
	INST_JUMPIFEQS( LABEL("cond%false"));
	INST_JUMP(LABEL("cond%true"));

    // nil
	INST_LABEL(LABEL("cond%nil"));
	INST_PUSHS(VAR_CODE("GF", "black%hole"));
	INST_JUMP(LABEL("cond%false"));
}


void genExitLabels() {
    INST_LABEL(LABEL("not%init"));
    INST_CLEARS();
    INST_POPFRAME();
    INST_EXIT(CONST_INT(SEMANTIC_VARIABLE_ERR));
}

void genVarDefs(ht_table_t *varSymtable) {
    if (varSymtable == NULL) {
        ERR_INTERNAL(genVarDefs, "symtable is NULL\n");
        return;
    }

    // create TF
    INST_CREATEFRAME();

    ht_item_t *tmp;

    for (int i = 0; i < HT_SIZE; i++) {
        tmp = varSymtable->items[i];
        while (tmp != NULL) {
            INST_DEFVAR(VAR_CODE("TF", tmp->identifier));
            tmp = tmp->next;
        }
    }

    // push TF to LF
    INST_PUSHFRAME();
}
