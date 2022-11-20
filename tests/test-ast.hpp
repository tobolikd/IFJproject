#ifndef IFJ_TEST_AST_HPP
#define  IFJ_TEST_AST_HPP

extern "C" {
    #include "../src/ast.h"
}

#include <gtest/gtest.h>

#include <iostream>

using std::cerr;
using std::endl;

class testBaseForAst : public ::testing::Test {
    protected:
        AST_item *testedItem;

        void SetUp() override {
            testedItem = NULL;
        }

        void TearDown() override {
            if (testedItem != NULL)
                ast_item_destr(testedItem);
        }

};


#endif // IFJ_TEST_AST_HPP
