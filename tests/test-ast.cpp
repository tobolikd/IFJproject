#include <gtest/gtest.h>

extern "C"
{
    #include "../src/ast.h"
    #include "../src/devel.h"
    #include "../src/symtable.h"
}

#include <string>
#include <cstdlib>
#include <iostream>
#include "test-ast.hpp"

class testAst : public testBaseForAst {};

TEST_F(testAst, initNoData) {
    testedItem = astItemConst(AST_ADD, NULL);

    ASSERT_FALSE(testedItem == NULL) << "Constructor returned NULL" << endl;
    EXPECT_EQ(testedItem->type, AST_ADD) << "Returned item has incorrect type" << endl;
    EXPECT_TRUE(testedItem->data == NULL);
}

TEST_F(testAst, initInt) {
    int a = 58;
    testedItem = astItemConst(AST_INT, &a);

    ASSERT_FALSE(testedItem == NULL) << "Constructor returned NULL" << endl;
    EXPECT_EQ(testedItem->type, AST_INT);
    ASSERT_FALSE(testedItem->data == NULL);
    EXPECT_EQ(58, testedItem->data->intValue);
}

TEST_F(testAst, initString) {
    char a[] = "sample string";
    testedItem = astItemConst(AST_STRING, a);

    ASSERT_FALSE(testedItem == NULL) << "Constructor returned NULL" << endl;
    EXPECT_EQ(testedItem->type, AST_STRING);
    ASSERT_FALSE(testedItem->data == NULL);
    EXPECT_EQ(0, strcmp(a, testedItem->data->stringValue));
}

TEST_F(testAst, initReturn) {
    bool a = true;
    testedItem = astItemConst(AST_RETURN_EXPR, &a);

    ASSERT_FALSE(testedItem == NULL) << "Constructor returned NULL" << endl;
    EXPECT_EQ(testedItem->type, AST_RETURN_EXPR);
    EXPECT_TRUE(testedItem->data == NULL);
}

TEST_F(testAst, initVar) {
    ht_item_t *a = (ht_item_t *) malloc(sizeof(ht_item_t));
    ASSERT_FALSE(a == NULL);
    testedItem = astItemConst(AST_VAR, a);

    ASSERT_FALSE(testedItem == NULL) << "Constructor returned NULL" << endl;
    EXPECT_EQ(testedItem->type, AST_VAR);
    ASSERT_FALSE(testedItem->data == NULL);
    EXPECT_TRUE(a == testedItem->data->variable);

    free(a);
}

TEST_F(testAst, fncCall) {
    ht_table_t *symtable = ht_init();
    char fncName[] = "sample_function";
    ht_insert(symtable, fncName, void_t, true);

    AST_function_call_data *data = fncCallDataConst(symtable, fncName);

    EXPECT_EQ(data->functionId, fncName);

    EXPECT_EQ(0, strcmp(fncName, data->function->identifier));
    EXPECT_TRUE(ht_search(symtable, fncName) == data->function);
    EXPECT_TRUE(NULL == data->params);

    int param1 = 123;
    double param2 = 1.2345;
    char param3[] = "sample string";
    ht_item_t param4;

    fncCallDataAddParam(data, AST_P_INT, &param1);
    AST_fnc_param *tmpParam = data->params;
    EXPECT_EQ(tmpParam->type, AST_P_INT);
    EXPECT_EQ(123, tmpParam->data->intValue);

    fncCallDataAddParam(data, AST_P_FLOAT, &param2);
    tmpParam = tmpParam->next;
    EXPECT_EQ(tmpParam->type, AST_P_FLOAT);
    EXPECT_DOUBLE_EQ(1.2345, tmpParam->data->floatValue);

    fncCallDataAddParam(data, AST_P_STRING, &param3);
    tmpParam = tmpParam->next;
    EXPECT_EQ(tmpParam->type, AST_P_STRING);
    EXPECT_FALSE(param3 == tmpParam->data->stringValue);
    EXPECT_EQ(0, strcmp(param3, tmpParam->data->stringValue));

    fncCallDataAddParam(data, AST_P_VAR, &param4);
    tmpParam = tmpParam->next;
    EXPECT_EQ(tmpParam->type, AST_P_VAR);
    EXPECT_TRUE(&param4 == tmpParam->data->variable);

    fncCallDataAddParam(data, AST_P_NULL, NULL);
    tmpParam = tmpParam->next;
    EXPECT_EQ(tmpParam->type, AST_P_NULL);
    EXPECT_TRUE(NULL == tmpParam->data);


    EXPECT_TRUE(NULL == tmpParam->next);

    testedItem = astItemConst(AST_FUNCTION_CALL, data);

    EXPECT_EQ(AST_FUNCTION_CALL, testedItem->type);
    EXPECT_TRUE(data == testedItem->data->functionCallData);

    ht_deleteAll(symtable);
}
