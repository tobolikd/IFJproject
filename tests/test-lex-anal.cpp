#include <gtest/gtest.h>

#include "../src/lex-anal.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

TEST(linkTest, lexAnal) {
    EXPECT_EQ(lexAnal('a'), 'a') << "Test function not found!";
}

namespace {
    class testCheckProlog : public ::testing::Test {
        protected:
            FILE *tmpFile;

            testCheckProlog() {}

            virtual ~testCheckProlog() {}

            void SetUp(const std::string fileInput) {
                tmpFile = tmpfile();

                if (tmpFile == NULL) {
                    printf("\n\nTEST ERROR - could not open tmpfile\n\n");
                    return;
                }

                fputs(fileInput.data(), tmpFile);

                rewind(tmpFile);
            }

            void TearDown() {
                fclose(tmpFile);
            }
};

TEST_F(testCheckProlog, correctValues) {
    
    std::string tmpString[] = {"<?php", "<?php ", "<?php\ndeclare()" };
    uint stringCount = sizeof(tmpString)/sizeof(tmpString[0]);

    for (int i = 0; i < stringCount; i++) {
        SetUp(tmpString[i]);
        EXPECT_EQ(0, checkProlog(tmpFile)) << "String \"" << tmpString[i] << "\" is valid prolog\n";
        if (i+1 != stringCount) { TearDown(); }
    }
}

TEST_F(testCheckProlog, incorrectValues) {
    
    std::string tmpString[] = {" <?php", "<?phpp", "", "<?ph", "<", "< ?php", "<\n?php"};
    uint stringCount = sizeof(tmpString)/sizeof(tmpString[0]);

    for (int i = 0; i < stringCount; i++) {
        SetUp(tmpString[i]);
        EXPECT_NE(0, checkProlog(tmpFile)) << "String \"" << tmpString[i] << "\" is invalid prolog\n";
        if (i+1 != stringCount) { TearDown(); }
    }
}

}; // namespace
