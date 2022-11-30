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
    // aux vars
    INST_DEFVAR(VAR_CODE("GF", "aux%1"));
    INST_DEFVAR(VAR_CODE("GF", "aux%2"));

    // jump over pregenerated code
    INST_JUMP(LABEL("end%pregenerated%end"));

    // generate functions (built in and aux)

    genResolveCondition();
    genImplicitConversions();
    genSemanticTypeCheck();
    genExitLabels();

    // end pregenerated
    INST_LABEL(LABEL("end%pregenerated%end"));

    COMMENT("END OF PREGENERATED PART");
    COMMENT("BEGINING OF PROGRAM PART");
}

void genResolveCondition() {
    INST_LABEL(LABEL("resolve%condition"));

	INST_POPS(AUX1); // value in aux1
	INST_TYPE(VAR_BLACKHOLE(), AUX1); // save type

	INST_JUMPIFEQ(LABEL("cond%int"), CONST_STRING("int"), VAR_BLACKHOLE());
	INST_JUMPIFEQ(LABEL("cond%bool"), CONST_STRING("bool"), VAR_BLACKHOLE());
	INST_JUMPIFEQ(LABEL("cond%float"), CONST_STRING("float"), VAR_BLACKHOLE());
	INST_JUMPIFEQ(LABEL("cond%string"), CONST_STRING("string"), VAR_BLACKHOLE());
	INST_JUMPIFEQ(LABEL("cond%nil"), CONST_STRING("nil"), VAR_BLACKHOLE());

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
	INST_JUMPIFEQ(LABEL("cond%false"), CONST_INT(0), AUX1);
	INST_JUMP(LABEL("cond%true"));

	// bool
	INST_LABEL(LABEL("cond%bool"));
    INST_PUSHS(AUX1);
    INST_RETURN();

	// float
	INST_LABEL(LABEL("cond%float"));
	INST_JUMPIFEQ(LABEL("cond%false"), CONST_FLOAT((double) 0), AUX1);
	INST_JUMP(LABEL("cond%true"));

	// string
	INST_LABEL(LABEL("cond%string"));
	INST_JUMPIFEQ(LABEL("cond%false"), CONST_STRING("0"), AUX1);
	INST_JUMPIFEQ(LABEL("cond%false"), CONST_STRING(""), AUX1);
	INST_JUMP(LABEL("cond%true"));

    // nil - always false
	INST_LABEL(LABEL("cond%nil"));
	INST_JUMP(LABEL("cond%false"));
}


void genExitLabels() {
    // uninitialized variable
    INST_LABEL(LABEL("not%init"));
    INST_DPRINT(CONST_STRING("use of not initialized var\n"));
    INST_CLEARS();
    INST_POPFRAME();
    INST_EXIT(CONST_INT(SEMANTIC_RUN_VARIABLE_ERR));

    // unsupported conversion type
    INST_LABEL(LABEL("unknown%type"));
    INST_DPRINT(CONST_STRING("not supported conversion type\n"));
    INST_CLEARS();
    INST_POPFRAME();
    INST_EXIT(CONST_INT(SEMANTIC_RUN_TYPE_ERR));

    // invalid parameter type in function call or return type
    INST_LABEL(LABEL("invalid%type"));
    INST_DPRINT(CONST_STRING("Unexpected type in function call parameter or return value.\n"));
    INST_CLEARS();
    INST_POPFRAME();
    INST_EXIT(CONST_INT(SEMANTIC_RUN_PARAMETER_ERR));
}

void genSemanticTypeCheck(){
    //int expected
    INST_LABEL(LABEL("type%check%int"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("invalid%type"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //float expected
    INST_LABEL(LABEL("type%check%float"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("invalid%type"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //string expected
    INST_LABEL(LABEL("type%check%string"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("invalid%type"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //nil expected
    INST_LABEL(LABEL("type%check%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("invalid%type"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //nil || int expected
    INST_LABEL(LABEL("type%check%int%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //nil || float expected
    INST_LABEL(LABEL("type%check%float%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_PUSHS(AUX1);
    INST_RETURN();

    //nil || string expected
    INST_LABEL(LABEL("type%check%string%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_PUSHS(AUX1);
    INST_RETURN();
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

void genImplicitConversions() {
    /* conv 2 values from stack
    * return to stack
    * int int - no change
    * float float - no change
    * int float / float int -> float float
    */
    INST_LABEL(LABEL("conv%arithmetic"));
    INST_POPS(AUX1);
    INST_POPS(AUX2);

    INST_TYPE(VAR_BLACKHOLE(), AUX1);

    INST_JUMPIFEQ(LABEL("conv%first%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%first%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%first%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // int ?
    INST_LABEL(LABEL("conv%first%int"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%int%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%int%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%int%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // float ?
    INST_LABEL(LABEL("conv%first%float"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%float%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%float%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%float%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // nil ?
    INST_LABEL(LABEL("conv%first%nil"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%nil%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%nil%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%nil%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // int int
    // float float
    INST_LABEL(LABEL("conv%int%int"));
    INST_LABEL(LABEL("conv%float%float"));
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // int float
    INST_LABEL(LABEL("conv%int%float"));
    INST_INT2FLOAT(AUX1, AUX1);
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // int nil
    INST_LABEL(LABEL("conv%int%nil"));
    INST_PUSHS(CONST_INT(0));
    INST_PUSHS(AUX1);
    INST_RETURN();

    // float int
    INST_LABEL(LABEL("conv%float%int"));
    INST_INT2FLOAT(AUX2, AUX2);
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // float nil
    INST_LABEL(LABEL("conv%float%nil"));
    INST_PUSHS(CONST_FLOAT((double) 0));
    INST_PUSHS(AUX1);
    INST_RETURN();

    // nil int
    INST_LABEL(LABEL("conv%nil%int"));
    INST_PUSHS(AUX2);
    INST_PUSHS(CONST_INT(0));
    INST_RETURN();

    // nil float
    INST_LABEL(LABEL("conv%nil%float"));
    INST_PUSHS(AUX2);
    INST_PUSHS(CONST_FLOAT((double) 0));
    INST_RETURN();

    // nil nil
    INST_LABEL(LABEL("conv%nil%nil"));
    INST_PUSHS(CONST_INT(0));
    INST_PUSHS(CONST_INT(0));
    INST_RETURN();


    /* conv%div
     *
     * converts 2 operands form stack to float
     * returns them on stack
     */
    INST_LABEL(LABEL("conv%div"));
    INST_POPS(AUX1);
    INST_CALL(LABEL("conv%to%float"));
    INST_PUSHS(AUX1);
    INST_CALL(LABEL("conv%to%float"));
    INST_RETURN();

    // aux function - convert stack top to float
    INST_LABEL(LABEL("conv%to%float"));
    INST_POPS(VAR_BLACKHOLE());

    INST_TYPE(VAR_BLACKHOLE(), VAR_BLACKHOLE());
    INST_JUMPIFEQ(LABEL("conv%to%float%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%to%float%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%to%float%int"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    INST_LABEL(LABEL("conv%to%float%int"));
    INST_INT2FLOAT(VAR_BLACKHOLE(), VAR_BLACKHOLE());
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();

    INST_LABEL(LABEL("conv%to%float%float"));
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();

    INST_LABEL(LABEL("conv%to%float%nil"));
    INST_PUSHS(CONST_FLOAT((double) 0));
    INST_RETURN();

    /* conv%concat
     *
     * checks 2 operands from stack
     * if ok returns them unchaged on stack
     * note: conversion might be implemented later
     */
    INST_LABEL(LABEL("conv%concat"));
    INST_POPS(AUX1);
    INST_POPS(AUX2);

    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFNEQ(LABEL("unknown%type"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_TYPE(VAR_BLACKHOLE(), AUX2);
    INST_JUMPIFNEQ(LABEL("unknown%type"), VAR_BLACKHOLE(), CONST_STRING("string"));

    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

}
