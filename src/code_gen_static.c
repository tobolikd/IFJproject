#include "code_gen_static.h"
#include "code_gen.h"
#include "symtable.h"

#include <stdio.h>

void genBuiltIns() {
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
    genDataTypeComparisons();
    genBuiltInFcs();

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
    // nil - always false
	INST_JUMPIFEQ(LABEL("cond%false"), CONST_STRING("nil"), VAR_BLACKHOLE());

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

    // missing return
    INST_LABEL(LABEL("no%return"));
    INST_DPRINT(CONST_STRING("No return from function was called\n"));
    INST_CLEARS();
    INST_POPFRAME();
    INST_EXIT(CONST_INT(SEMANTIC_PARAMETER_ERR));
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
    INST_PUSHS(AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_RETURN();

    //nil || float expected
    INST_LABEL(LABEL("type%check%float%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_PUSHS(AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_RETURN();

    //nil || string expected
    INST_LABEL(LABEL("type%check%string%nil"));
    INST_POPS(AUX1);
    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_PUSHS(AUX1);
    INST_JUMPIFNEQ(LABEL("type%check%nil"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_RETURN();
}

void genDataTypeComparisons(){
    INST_LABEL(LABEL("type%cmp"));

	INST_POPS(AUX1); // read value of operand
	INST_POPS(AUX2); // read value of operand
	INST_PUSHS(AUX2);
	INST_PUSHS(AUX1);

	INST_TYPE(AUX1, AUX1);//get types
	INST_TYPE(AUX2, AUX2);

	INST_JUMPIFEQ(LABEL("expect%int"), CONST_STRING("int"), AUX1);
	INST_JUMPIFEQ(LABEL("expect%bool"), CONST_STRING("bool"), AUX1);
	INST_JUMPIFEQ(LABEL("expect%float"), CONST_STRING("float"), AUX1);
	INST_JUMPIFEQ(LABEL("expect%string"), CONST_STRING("string"), AUX1);
	INST_JUMPIFEQ(LABEL("expect%nil"), CONST_STRING("nil"), AUX1);

    //int expected
    INST_LABEL(LABEL("expect%int"));
    INST_JUMPIFNEQ(LABEL("push%false"), AUX2, CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("push%true"), AUX2, CONST_STRING("int"));
    INST_RETURN();
    //bool expected
    INST_LABEL(LABEL("expect%bool"));
    INST_JUMPIFNEQ(LABEL("push%false"), AUX2, CONST_STRING("bool"));
    INST_JUMPIFEQ(LABEL("push%true"), AUX2, CONST_STRING("bool"));
    INST_RETURN();
    //float expected
    INST_LABEL(LABEL("expect%float"));
    INST_JUMPIFNEQ(LABEL("push%false"), AUX2, CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("push%true"), AUX2, CONST_STRING("float"));
    INST_RETURN();
    //string expected
    INST_LABEL(LABEL("expect%string"));
    INST_JUMPIFNEQ(LABEL("push%false"), AUX2, CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("push%true"), AUX2, CONST_STRING("string"));
    INST_RETURN();
    //nil expected
    INST_LABEL(LABEL("expect%nil"));
    INST_JUMPIFNEQ(LABEL("push%false"), AUX2, CONST_STRING("nil"));
    INST_POPS(VAR_BLACKHOLE());
    INST_POPS(VAR_BLACKHOLE());
    INST_PUSHS(CONST_BOOL("true"));
    INST_RETURN();

    INST_LABEL(LABEL("push%false"));
    INST_POPS(VAR_BLACKHOLE());
    INST_POPS(VAR_BLACKHOLE());
    INST_PUSHS(CONST_BOOL("false"));
    INST_RETURN();

    INST_LABEL(LABEL("push%true"));
    INST_EQS();
    INST_RETURN();
}

void genVarDefs(ht_table_t *varSymtable, ht_item_t* function) {
    if (varSymtable == NULL) {
        ERR_INTERNAL(genVarDefs, "symtable is NULL\n");
        return;
    }

    ht_item_t *tmpVar;
    param_info_t *firstParam;
    param_info_t *tmpParam;
    bool varIsParam;

    if (function == NULL) {
        firstParam = NULL;
    } else {
        firstParam = function->fnc_data.params;
    }

    for (int i = 0; i < HT_SIZE; i++) {
        tmpVar = varSymtable->items[i];
        while (tmpVar != NULL) {
            // check if variable isnt predefined parameter
            varIsParam = false;
            tmpParam = firstParam;
            while (tmpParam != NULL) {
                if (!strcmp(tmpParam->varId, tmpVar->identifier)) {
                    varIsParam = true;
                    break;
                }
                tmpParam = tmpParam->next;
            }

            if (!varIsParam) { // dont define parametres
                INST_DEFVAR(VAR_CODE("LF", tmpVar->identifier));
            }
            tmpVar = tmpVar->next;
        }
    }
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

    INST_JUMPIFEQ(LABEL("conv%arithm%first%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%arithm%first%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%arithm%first%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // int ?
    INST_LABEL(LABEL("conv%arithm%first%int"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%arithm%int%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%arithm%int%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%arithm%int%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // float ?
    INST_LABEL(LABEL("conv%arithm%first%float"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%arithm%float%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%arithm%float%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%arithm%float%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // nil ?
    INST_LABEL(LABEL("conv%arithm%first%nil"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);

    INST_JUMPIFEQ(LABEL("conv%arithm%nil%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%arithm%nil%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%arithm%nil%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // int int
    // float float
    INST_LABEL(LABEL("conv%arithm%int%int"));
    INST_LABEL(LABEL("conv%arithm%float%float"));
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // int float
    INST_LABEL(LABEL("conv%arithm%int%float"));
    INST_INT2FLOAT(AUX1, AUX1);
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // int nil
    INST_LABEL(LABEL("conv%arithm%int%nil"));
    INST_PUSHS(CONST_INT(0));
    INST_PUSHS(AUX1);
    INST_RETURN();

    // float int
    INST_LABEL(LABEL("conv%arithm%float%int"));
    INST_INT2FLOAT(AUX2, AUX2);
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // float nil
    INST_LABEL(LABEL("conv%arithm%float%nil"));
    INST_PUSHS(CONST_FLOAT((double) 0));
    INST_PUSHS(AUX1);
    INST_RETURN();

    // nil int
    INST_LABEL(LABEL("conv%arithm%nil%int"));
    INST_PUSHS(AUX2);
    INST_PUSHS(CONST_INT(0));
    INST_RETURN();

    // nil float
    INST_LABEL(LABEL("conv%arithm%nil%float"));
    INST_PUSHS(AUX2);
    INST_PUSHS(CONST_FLOAT((double) 0));
    INST_RETURN();

    // nil nil
    INST_LABEL(LABEL("conv%arithm%nil%nil"));
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
    INST_PUSHS(VAR_BLACKHOLE());

    INST_TYPE(VAR_BLACKHOLE(), VAR_BLACKHOLE());
    INST_JUMPIFEQ(LABEL("conv%to%float%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%to%float%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%to%float%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // int on stack
    INST_LABEL(LABEL("conv%to%float%int"));
    INST_POPS(VAR_BLACKHOLE());
    INST_INT2FLOAT(VAR_BLACKHOLE(), VAR_BLACKHOLE());
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();

    // float on stack
    INST_LABEL(LABEL("conv%to%float%float"));
    INST_RETURN();

    // nil on stack
    INST_LABEL(LABEL("conv%to%float%nil"));
    INST_POPS(VAR_BLACKHOLE());
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
    INST_JUMPIFEQ(LABEL("conv%concat%first%string"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("conv%concat%first%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // string ?
    INST_LABEL(LABEL("conv%concat%first%string"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);
    INST_JUMPIFEQ(LABEL("conv%concat%string%string"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("conv%concat%string%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // nil ?
    INST_LABEL(LABEL("conv%concat%first%nil"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);
    INST_JUMPIFEQ(LABEL("conv%concat%nil%string"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("conv%concat%nil%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));

    // string string
    INST_PUSHS(CONST_STRING(""));
    INST_LABEL(LABEL("conv%concat%string%string"));
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_RETURN();

    // string nil
    INST_LABEL(LABEL("conv%concat%string%nil"));
    INST_PUSHS(CONST_STRING(""));
    INST_PUSHS(AUX1);
    INST_RETURN();

    // nil string
    INST_LABEL(LABEL("conv%concat%nil%string"));
    INST_PUSHS(AUX2);
    INST_PUSHS(CONST_STRING(""));
    INST_RETURN();

    // nil nil
    INST_LABEL(LABEL("conv%concat%nil%nil"));
    INST_PUSHS(CONST_STRING(""));
    INST_PUSHS(CONST_STRING(""));
    INST_RETURN();

    /* conv%rel
     *
     * converts 2 operands from stack to the same type
     * uses same logic as arithmetic operations + string type
     */
    INST_LABEL(LABEL("conv%rel"));
    INST_POPS(AUX1);
    INST_POPS(AUX2);

    INST_TYPE(VAR_BLACKHOLE(), AUX1);
    INST_JUMPIFEQ(LABEL("conv%concat%first%string"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("conv%rel%first%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    // int or float - convert using arithmetic logic
    INST_PUSHS(AUX2);
    INST_PUSHS(AUX1);
    INST_JUMP(LABEL("conv%arithmetic"));

    // nil ?
    INST_LABEL(LABEL("conv%rel%first%nil"));

    INST_TYPE(VAR_BLACKHOLE(), AUX2);
    INST_JUMPIFEQ(LABEL("conv%concat%nil%string"), VAR_BLACKHOLE(), CONST_STRING("string"));
    INST_JUMPIFEQ(LABEL("conv%arithm%nil%int"), VAR_BLACKHOLE(), CONST_STRING("int"));
    INST_JUMPIFEQ(LABEL("conv%arithm%nil%float"), VAR_BLACKHOLE(), CONST_STRING("float"));
    INST_JUMPIFEQ(LABEL("conv%arithm%nil%nil"), VAR_BLACKHOLE(), CONST_STRING("nil"));
    INST_JUMP(LABEL("unknown%type"));
}
void genBuiltInFcs(){
    //
    //readi
    //
    //
    INST_LABEL(LABEL("readi"));
    INST_READ(VAR_BLACKHOLE(), "int");
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();
    //
    //readf
    //
    INST_LABEL(LABEL("readf"));
    INST_READ(VAR_BLACKHOLE(), "float");
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();
    //
    //reads
    //
    INST_LABEL(LABEL("reads"));
    INST_READ(VAR_BLACKHOLE(), "string");
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();
    //
    //chr
    //
    INST_LABEL(LABEL("chr"));
    INST_LABEL(LABEL("type%ok"));
    INST_INT2CHAR(VAR_BLACKHOLE(), VAR_CODE("LF", "i"));
    INST_PUSHS(VAR_BLACKHOLE());//pushing back to stack, where we will compute with this in next step.
    INST_RETURN();
    //
    //ord fction
    //
    INST_LABEL(LABEL("ord"));
    INST_DEFVAR(VAR_CODE("LF","ord%cond"));
    INST_TYPE(VAR_CODE("LF","ord%cond"), VAR_CODE("LF","c"));
    INST_JUMPIFNEQ(LABEL("ord%len%not0"),VAR_CODE("LF","c"), CONST_STRING(""));
    INST_PUSHS(CONST_INT(0));//returns 0 if the list length is 0.
    INST_RETURN();
    INST_LABEL(LABEL("ord%len%not0"));
    INST_STRI2INT(VAR_BLACKHOLE(),VAR_CODE("LF", "c"), CONST_INT(0));
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();
    //
    //substr
    //
    INST_LABEL(LABEL("substring"));
    COMMENT("generating substring");
    INST_DEFVAR(VAR_CODE("LF", "length%of%string"));
    INST_MOVE(VAR_CODE("LF", "length%of%string"), CONST_NIL()); //init
    INST_STRLEN(VAR_CODE("LF", "length%of%string"),VAR_CODE("LF", "s"));

    //checking condition
    INST_DPRINT(CONST_STRING("DECIDING\n"));
    INST_LT(VAR_BLACKHOLE(), VAR_CODE("LF", "i"), CONST_INT(0));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));   //if i < 0,  jump and return null
    INST_LT(VAR_BLACKHOLE(), VAR_CODE("LF", "j"), CONST_INT(0));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));   //if j < 0,  jump and return null
    INST_LT(VAR_BLACKHOLE(), VAR_CODE("LF", "j"), VAR_CODE("LF", "i"));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));   //if j < i,  jump and return null
    INST_GT(VAR_BLACKHOLE(), VAR_CODE("LF", "j"), VAR_CODE("LF", "length%of%string"));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));   //if strlen(s) < j,  jump and return null
    INST_GT(VAR_BLACKHOLE(), VAR_CODE("LF", "i"), VAR_CODE("LF", "length%of%string"));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));   //if strlen(s) < i,  jump and return null
    INST_EQ(VAR_BLACKHOLE(), VAR_CODE("LF", "i"), VAR_CODE("LF", "length%of%string"));
    INST_JUMPIFEQ(LABEL("is%lt0"),VAR_BLACKHOLE(), CONST_BOOL("true"));    //if strlen(s) = i,  jump and return null
    //now we need to return the substring
    INST_DEFVAR(VAR_CODE("LF","max%len"));
    INST_DEFVAR(VAR_CODE("LF","max%index"));
    INST_DEFVAR(VAR_CODE("LF", "return%string"));
    INST_MOVE(VAR_CODE("LF", "return%string"), CONST_STRING(""));
    INST_DEFVAR(VAR_CODE("LF", "start%length"));
    INST_MOVE(VAR_CODE("LF", "start%length"),VAR_CODE("LF", "i"));
    INST_SUB(VAR_CODE("LF","max%len"), VAR_CODE("LF", "j"), VAR_CODE("LF", "start%length"));
    //INST_ADD(VAR_CODE("LF","max%len"), VAR_CODE("LF", "j"),CONST_INT(1));
    INST_LABEL(LABEL("for%cycle"));
    INST_JUMPIFEQ(LABEL("for%cycle%end"), VAR_CODE("LF", "j"), VAR_CODE("LF", "start%length"));//is starting index now the same as ending index?
    INST_GETCHAR(VAR_BLACKHOLE(), VAR_CODE("LF", "s"), VAR_CODE("LF", "start%length"));
    INST_CONCAT(VAR_CODE("LF", "return%string"), VAR_CODE("LF", "return%string"), VAR_BLACKHOLE());
    INST_ADD(VAR_CODE("LF", "start%length"),VAR_CODE("LF", "start%length"), CONST_INT(1));
    INST_JUMP(LABEL("for%cycle"));
    INST_LABEL(LABEL("for%cycle%end"));
    INST_PUSHS(VAR_CODE("LF", "return%string"));
    INST_RETURN();

    INST_LABEL(LABEL("is%lt0"));
    INST_PUSHS(CONST_NIL());//returns null
    INST_RETURN();
    //
    //write is done at code-gen.c
    //
    //
    //strlen
    //
    INST_LABEL(LABEL("strlen"));
    INST_STRLEN(VAR_BLACKHOLE(),VAR_CODE("LF", "s"));
    INST_PUSHS(VAR_BLACKHOLE());
    INST_RETURN();
}
