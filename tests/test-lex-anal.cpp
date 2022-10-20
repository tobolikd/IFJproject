#include <gtest/gtest.h>

extern "C" {
    #include "../src/lex-anal.h"

    #include <stdio.h>
    #include <stdlib.h>
}

#include <string>

#include "test-lex-anal.hpp"

TEST(linkTest, lexAnal) {
    EXPECT_EQ(lexAnal('a'), 'a') << "Test function not found!";
}

class testCheckProlog : public ::testing::TestWithParam<std::tuple<int, std::string>> {};

INSTANTIATE_TEST_CASE_P(prologValues, testCheckProlog,
                        testing::Values(
                            std::make_tuple(0, "<?php"),
                            std::make_tuple(0, "<?php "),
                            std::make_tuple(0, "<?php\ndeclare()"),
                            std::make_tuple(1, " <?php"),
                            std::make_tuple(1, "<?phpp"),
                            std::make_tuple(1, ""),
                            std::make_tuple(1, "<?ph"),
                            std::make_tuple(1, "<"),
                            std::make_tuple(1, "< ?php"),
                            std::make_tuple(1, "<\n?php"),
                            std::make_tuple(1, "<<"),
                            std::make_tuple(1, "<?php?>")
                            )
                        );

TEST_P(testCheckProlog, returnValue) {
    int returnValue= std::get<0>(GetParam());
    const char * in = std::get<1>(GetParam()).data();

    FILE *tmpFile = prepTmpFile(in);
    ASSERT_FALSE(tmpFile == NULL) << "INTERNAL TEST ERROR - failed to allocate file" << std::endl;
    EXPECT_EQ(returnValue, checkProlog(tmpFile)) << "Processed input: \"" << in << "\"" << std::endl;
    fclose(tmpFile);
}


