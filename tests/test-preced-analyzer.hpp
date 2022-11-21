#ifndef IFJ_TEST_PRECED_ANALYZER_H
#define IFJ_TEST_PRECED_ANALYZER_H 1

extern "C"
{
    #include "../src/lex-analyzer.h"
    #include "../src/preced-analyzer.h"
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



#endif // IFJ_TEST_PRECED_ANALYZER_H
