#include <gtest/gtest.h>
#include "common.h"
#include "value.h"

TEST(ValueTest, NullPtrAssertions)
{
    Value value(nullptr);
    EXPECT_TRUE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_FALSE(value.isString());

    EXPECT_NO_THROW({ const auto nullValue = (nullptr_t)value; });
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
}

TEST(ValueTest, BoolAssertions)
{
    Value value(true);
    EXPECT_FALSE(value.isNull());
    EXPECT_TRUE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_FALSE(value.isString());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto boolValue = (bool)value; });
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
}

TEST(ValueTest, DoubleAssertions)
{
    Value value(1.0);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_TRUE(value.isDouble());
    EXPECT_FALSE(value.isString());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto doubleValue = (double)value; });
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
}

TEST(ValueTest, StdStringAssertions)
{
    Value value(std::string("string"));
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isString());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
}

TEST(ValueTest, CStringAssertions)
{
    Value value("string");
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_TRUE(value.isString());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
}