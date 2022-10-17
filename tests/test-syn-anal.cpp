#include <gtest/gtest.h>
#include "../src/syn-anal.h"

TEST(linkTest, synAnal) {
    EXPECT_EQ(synAnal('b'), 'b');
}
