#include "environment.h"

#include <gtest/gtest.h>

#include "common.h"

class EnvironmentTest : public ::testing::Test {};

TEST_F(EnvironmentTest, TestBasic) {
  auto env = std::make_shared<Environment>();
  EXPECT_EQ(*env->get("id1"), *NULL_OBJECT_PTR);
  env->set("id1", TRUE_OBJECT_PTR);
  EXPECT_EQ(*env->get("id1"), *TRUE_OBJECT_PTR);
}

TEST_F(EnvironmentTest, TestEnclosing) {
  auto enclosingEnv = std::make_shared<Environment>();
  auto innerEnv = std::make_shared<Environment>(enclosingEnv);

  EXPECT_EQ(*innerEnv->get("id1"), *NULL_OBJECT_PTR);
  innerEnv->set("id1", TRUE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("id1"), *TRUE_OBJECT_PTR);

  enclosingEnv->set("globalId", TRUE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("globalId"), *TRUE_OBJECT_PTR);
  innerEnv->set("globalId", FALSE_OBJECT_PTR);
  EXPECT_EQ(*enclosingEnv->get("globalId"), *FALSE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("globalId"), *FALSE_OBJECT_PTR);
}

TEST_F(EnvironmentTest, TestShadowing) {
  auto enclosingEnv = std::make_shared<Environment>();
  auto innerEnv = std::make_shared<Environment>(enclosingEnv);

  EXPECT_EQ(*innerEnv->get("id1"), *NULL_OBJECT_PTR);
  innerEnv->set("id1", TRUE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("id1"), *TRUE_OBJECT_PTR);

  enclosingEnv->set("id1", FALSE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("id1"), *TRUE_OBJECT_PTR);
  EXPECT_EQ(*enclosingEnv->get("id1"), *FALSE_OBJECT_PTR);
}

TEST_F(EnvironmentTest, TestDeclare) {
  auto enclosingEnv = std::make_shared<Environment>();
  enclosingEnv->set("id1", FALSE_OBJECT_PTR);

  auto innerEnv = std::make_shared<Environment>(enclosingEnv);
  EXPECT_EQ(*innerEnv->get("id1"), *FALSE_OBJECT_PTR);
  innerEnv->declare("id1", TRUE_OBJECT_PTR);
  EXPECT_EQ(*innerEnv->get("id1"), *TRUE_OBJECT_PTR);
  EXPECT_EQ(*enclosingEnv->get("id1"), *FALSE_OBJECT_PTR);
}