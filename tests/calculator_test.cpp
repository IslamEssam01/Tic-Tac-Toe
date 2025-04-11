#include <gtest/gtest.h>
#include "calculator.hpp"

class CalculatorTest : public ::testing::Test {
protected:
    Calculator calc;
};

TEST_F(CalculatorTest, Addition) {
    EXPECT_DOUBLE_EQ(calc.add(2, 3), 5);
    EXPECT_DOUBLE_EQ(calc.add(-1, 1), 0);
    EXPECT_DOUBLE_EQ(calc.add(0, 0), 0);
}

TEST_F(CalculatorTest, Subtraction) {
    EXPECT_DOUBLE_EQ(calc.subtract(5, 3), 2);
    EXPECT_DOUBLE_EQ(calc.subtract(1, 1), 0);
    EXPECT_DOUBLE_EQ(calc.subtract(0, 5), -5);
}

TEST_F(CalculatorTest, Multiplication) {
    EXPECT_DOUBLE_EQ(calc.multiply(2, 3), 6);
    EXPECT_DOUBLE_EQ(calc.multiply(-2, 3), -6);
    EXPECT_DOUBLE_EQ(calc.multiply(0, 5), 0);
}

TEST_F(CalculatorTest, Division) {
    EXPECT_DOUBLE_EQ(calc.divide(6, 2), 3);
    EXPECT_DOUBLE_EQ(calc.divide(5, 2), 2.5);
    EXPECT_DOUBLE_EQ(calc.divide(0, 5), 0);
    EXPECT_THROW(calc.divide(5, 0), std::invalid_argument);
}
