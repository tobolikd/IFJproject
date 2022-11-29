#ifndef IFJ_TEST_PRECED_ANALYZER_H
#define IFJ_TEST_PRECED_ANALYZER_H 1

extern "C"
{
    #include "../src/symtable.h"
    #include "../src/stack.h"
    #include "../src/lex-analyzer.h"
    #include "../src/preced-analyzer-data.h"
    #include "test-lex-analyzer.hpp" //prepTmpFile
}

#include <string>
#include <iostream>
#include <gtest/gtest.h>

#include <cstdlib>


using std::tuple;
using std::to_string;
using std::get;
using std::cerr;
using std::vector;
using std::pair;
using std::endl;
using std::string;
using std::array;


class testBase : public::testing::TestWithParam<tuple<bool,string>>
{
    protected:
        FILE *tmpFile;
        TokenList *testList;
        bool expectedValue;
        string dataIn;
        int index;

        void SetUp() override
        {
            index = 0;

            expectedValue = get<0>(GetParam());

            dataIn = "<?php declare(strict_types=1);" + get<1>(GetParam());

            tmpFile = prepTmpFile(dataIn.data());
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file." << endl;

            testList = lexAnalyser(tmpFile);
            ASSERT_FALSE(testList == NULL) << "INTERNAL TEST ERROR - scanner failed to read input data." << endl;
        }

        void TearDown() override
        {
            if (tmpFile != NULL)
                fclose(tmpFile);
            listDtor(testList);
        }
};


class testBaseAST : public::testing::TestWithParam<tuple<bool,string>>
{
    protected:
        FILE *tmpFile;
        TokenList *testList;
        bool expectedValue;
        string dataIn;
        int index;
        ht_table_t *testTableVar;
        stack_ast_t testStack;

        void SetUp() override
        {
            stack_ast_init(&testStack);//init stack
            ASSERT_FALSE(&testStack == NULL) << "INTERNAL TEST ERROR - stack init error." << endl;

            testTableVar = ht_init();//init symtables
            ASSERT_FALSE(testTableVar == NULL) << "INTERNAL TEST ERROR - symtable init error." << endl;
            fncTable = ht_init();
            ASSERT_FALSE(fncTable == NULL) << "INTERNAL TEST ERROR - symtable init error." << endl;

            char a[] = "a";
            char b[] = "b";
            char foo[] = "foo";
            char doe[] = "doe";
            char poo[] = "poo";
            char param[] = "param";

            ht_insert(testTableVar,a,int_t,false);//insert to symtables
            ASSERT_FALSE(ht_search(testTableVar,a) == NULL);
            ht_insert(testTableVar,b,int_t,false);
            ASSERT_FALSE(ht_search(testTableVar,b) == NULL);

            ht_param_append(ht_insert(fncTable,foo,int_t,true),param,float_t);
            ASSERT_FALSE(ht_search(fncTable,foo) == NULL);
            ASSERT_FALSE(ht_search(fncTable,foo)->fnc_data.params == NULL);
           
            ht_insert(fncTable,poo,int_t,true);
            ASSERT_FALSE(ht_search(fncTable,poo) == NULL);
            ht_param_append(ht_search(fncTable,poo),param,float_t);
            ht_param_append(ht_search(fncTable,poo),param,float_t);

            ht_insert(fncTable,doe,int_t,true);
            ASSERT_FALSE(ht_search(fncTable,doe) == NULL);

            index = 0;

            expectedValue = get<0>(GetParam());//true/false

            dataIn = "<?php declare(strict_types=1);" + get<1>(GetParam());//input expression

            tmpFile = prepTmpFile(dataIn.data());
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file." << endl;

            testList = lexAnalyser(tmpFile);
            ASSERT_FALSE(testList == NULL) << "INTERNAL TEST ERROR - scanner failed to read input data." << endl;
        }

        void TearDown() override
        {
            while (!stack_ast_empty(&testStack)) //pop all
                stack_ast_pop(&testStack);

            ht_delete_all(fncTable);
            ht_delete_all(testTableVar);
            if (tmpFile != NULL)
                fclose(tmpFile);
            listDtor(testList);
        }
};

class CheckDoubleValue : public::testing::TestWithParam<tuple<double,string>>
{
    protected:
        FILE *tmpFile;
        TokenList *testList;
        string dataIn;
        double expectedValue;
        int index;
        ht_table_t *testTableVar;
        stack_ast_t testStack;

        void SetUp() override
        {
            stack_ast_init(&testStack);//init stack
            ASSERT_FALSE(&testStack == NULL) << "INTERNAL TEST ERROR - stack init error." << endl;

            testTableVar = ht_init();//init symtables
            ASSERT_FALSE(testTableVar == NULL) << "INTERNAL TEST ERROR - symtable init error." << endl;
            fncTable = ht_init();
            ASSERT_FALSE(fncTable == NULL) << "INTERNAL TEST ERROR - symtable init error." << endl;

            index = 0;

            expectedValue = get<0>(GetParam());

            dataIn = "<?php declare(strict_types=1);" + get<1>(GetParam());//input expression

            tmpFile = prepTmpFile(dataIn.data());
            ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file." << endl;

            testList = lexAnalyser(tmpFile);
            ASSERT_FALSE(testList == NULL) << "INTERNAL TEST ERROR - scanner failed to read input data." << endl;
        }

        void TearDown() override
        {
            while (!stack_ast_empty(&testStack)) //pop all
                stack_ast_pop(&testStack);

            ht_delete_all(fncTable);
            ht_delete_all(testTableVar);
            if (tmpFile != NULL)
                fclose(tmpFile);
            listDtor(testList);
        }
};


#endif // IFJ_TEST_PRECED_ANALYZER_H
