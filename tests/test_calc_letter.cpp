#include <gtest/gtest.h>
#include "../src/calc_letter.h"

TEST(CalcLetterTest, A_grade) {
    EXPECT_EQ('A', calc_letter(100));
    EXPECT_EQ('A', calc_letter(95));
    EXPECT_EQ('A', calc_letter(90));
}

TEST(CalcLetterTest, B_grade) {
    EXPECT_EQ('B', calc_letter(89));
    EXPECT_EQ('B', calc_letter(85));
    EXPECT_EQ('B', calc_letter(80));
}

TEST(CalcLetterTest, C_grade) {
    EXPECT_EQ('C', calc_letter(79));
    EXPECT_EQ('C', calc_letter(75));
    EXPECT_EQ('C', calc_letter(70));
}

TEST(CalcLetterTest, D_grade) {
    EXPECT_EQ('D', calc_letter(69));
    EXPECT_EQ('D', calc_letter(65));
    EXPECT_EQ('D', calc_letter(60));
}

TEST(CalcLetterTest, F_grade) {
    EXPECT_EQ('F', calc_letter(59));
    EXPECT_EQ('F', calc_letter(30));
    EXPECT_EQ('F', calc_letter(0));
    EXPECT_EQ('F', calc_letter(-10));
}

TEST(CalcLetterTest, BoundaryValues) {
    EXPECT_EQ('A', calc_letter(90));
    EXPECT_EQ('B', calc_letter(80));
    EXPECT_EQ('C', calc_letter(70));
    EXPECT_EQ('D', calc_letter(60));
}

TEST(CalcLetterTest, EdgeBelowBoundary) {
    EXPECT_EQ('B', calc_letter(89.999));
    EXPECT_EQ('D', calc_letter(69.999));
    EXPECT_EQ('F', calc_letter(59.999));
    EXPECT_EQ('F', calc_letter(49.999));
}
