#include <gtest/gtest.h>

extern "C" {
    #include "../src/syn-anal.h"
}

TEST(linkTest, synAnal) {
    EXPECT_EQ(synAnal('b'), 'b');
}
