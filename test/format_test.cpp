#include "gechtest/include/gechtest.hpp"
#include "../include/idk.hpp"

TEST(TEST_CASE) {
    ASSERT_EQ(idk::format("{}", "gech"), "gech")
    ASSERT_EQ(idk::format("zero is {}", 0), "zero is 0")
    ASSERT_EQ(idk::format("{Z: $}", 5), "101")
    ASSERT_EQ(idk::format("{L}", "GECH"), "gech")
    ASSERT_EQ(idk::format("{U: 1}", "TEST", "gech"), "GECH")
    ASSERT_EQ(idk::format("{A}", -500), "500")
    ASSERT_EQ(idk::format("G{D:0:2}ch", 'e'), "Geech")
    ASSERT_EQ(idk::format("G{D:0:2:'!'}ch", 'e'), "Ge!e!ch")
    ASSERT_EQ(idk::format("{1}{0}", '2', '4'), "42")
}

TEST_MAIN