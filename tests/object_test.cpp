#include "object.h"

#include <gtest/gtest.h>

#include "common.h"

class ObjectTest : public ::testing::Test {};

TEST_F(ObjectTest, IntegerObjectTest) {
  IntegerObject i0(0);
  IntegerObject i1(1);
  IntegerObject i2(2);
  IntegerObject i3(1);

  EXPECT_TRUE(i0.isFalsey());
  EXPECT_FALSE(i0.isTruthy());
  EXPECT_EQ(i0.Value, 0);
  EXPECT_NE(i0, i1);
  EXPECT_EQ(i1, i3);
  EXPECT_NE(i1, i2);
  EXPECT_EQ(i1.Value, 1);
  EXPECT_EQ(i2.Value, 2);
  EXPECT_EQ(i1.Type, i2.Type);
  EXPECT_TRUE(i1.isTruthy());
  EXPECT_FALSE(i1.isFalsey());
}

TEST_F(ObjectTest, StringObjectTest) {
  StringObject s0("");
  StringObject s1("a");
  StringObject s2("b");
  StringObject s3("a");

  EXPECT_TRUE(s0.isFalsey());
  EXPECT_FALSE(s0.isTruthy());
  EXPECT_EQ(s0.Value, "");
  EXPECT_EQ(s1, s3);
  EXPECT_NE(s1, s2);
  EXPECT_EQ(s1.Value, "a");
  EXPECT_EQ(s2.Value, "b");
  EXPECT_EQ(s1.Type, s2.Type);
  EXPECT_TRUE(s1.isTruthy());
  EXPECT_FALSE(s1.isFalsey());
}