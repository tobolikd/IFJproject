#include <gtest/gtest.h>

extern "C"
{
    #include "../src/preced-analyzer.c"
}

#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>

#include "test-preced-analyzer.hpp"

using std::make_tuple;
using std::make_pair;
using std::endl;


class testCheckReturnValue : public testBase {};



TEST_P(testCheckReturnValue, expectedValue)
{
    EXPECT_EQ(expectedValue,precedAnalyzer(testList,1));
}

INSTANTIATE_TEST_SUITE_P(CONST_CORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(true, "2"),
        make_tuple(true, "2+2"),
        make_tuple(true, "2+2+2"),
        make_tuple(true, "(2+2)"),
        make_tuple(true, "(2+2)+2"),
        make_tuple(true, "(2)+2"),
        make_tuple(true, "-2+2"),
        make_tuple(true, "(-2)"),
        make_tuple(true, "2*2"),
        make_tuple(true, "2*2+2"),
        make_tuple(true, "2*2+2-2\\2"),
        make_tuple(true, "(2)+(2)*(2)\\(2)"),
        make_tuple(true, "-2+(-2)"),
        make_tuple(true, "-2*(-2)"),
        make_tuple(true, "-(+2)-(+2)")
    )
);

INSTANTIATE_TEST_SUITE_P(CONST_INCORRECT, testCheckReturnValue,
    testing::Values(
        make_tuple(false, "()"),
        make_tuple(false, "2++"),
        make_tuple(false, "2++2"),
        make_tuple(false, "(2+2)2"),
        make_tuple(false, "2(2-2)"),
        make_tuple(false, "(2+2)*2*"),
        make_tuple(false, "2(+)2"),
        make_tuple(false, "(-2))"),
        make_tuple(false, "(2*2"),
        make_tuple(false, "2+-2"),
        make_tuple(false, "2*+2"),
        make_tuple(false, "2\\"),
        make_tuple(false, "{2+2}"),
        make_tuple(false, "[2*(-2)]"),
        make_tuple(false, "(+2)(-2)")
    )
);


/* VARIABLES_CORRECT */

/* VARIABLES_INCORRECT */

/* LOGICAL_OPERATORS_CORRECT */

/* LOGICAL_OPERATORS_INRRECT */


/* TEST RETURNED AST NOD */