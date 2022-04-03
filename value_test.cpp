#include <gtest/gtest.h>
#include "common.h"
#include "value.h"
#include "function.h"

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

    EXPECT_EQ((std::string)value, "<nil>");
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

    EXPECT_EQ((std::string)value, "true");
    EXPECT_EQ((std::string)Value(false), "false");
}

TEST(ValueTest, DoubleAssertions)
{
    Value value(1.2345);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_TRUE(value.isDouble());
    EXPECT_FALSE(value.isString());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto doubleValue = (double)value; });
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });

    EXPECT_EQ((std::string)value, "1.2345");
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

    EXPECT_EQ((std::string)value, "string");
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

    EXPECT_EQ((std::string)value, "string");
}

TEST(ValueTest, ObjectAssertions)
{
    auto object = std::make_shared<Object>();
    Value value(object);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_FALSE(value.isString());
    EXPECT_TRUE(value.isObject());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
    EXPECT_NO_THROW({ const auto objectValue = (ObjectPtr)value; });

    {
        const auto object2 = (ObjectPtr)value;
        // expect 3 references, one "object", one boxed in "value" and "object2"
        EXPECT_EQ(object2.use_count(), 3);
    }
    // "object2" is gone
    EXPECT_EQ(object.use_count(), 2);
    EXPECT_EQ((std::string)value, "");
}

TEST(ValueTest, FunctionAssertions)
{
    auto function = std::make_shared<Function>(FunctionType::TYPE_FUNCTION, "testFunction", 1);
    auto object = std::static_pointer_cast<Object>(function);
    Value value(object);
    EXPECT_FALSE(value.isNull());
    EXPECT_FALSE(value.isBool());
    EXPECT_FALSE(value.isDouble());
    EXPECT_FALSE(value.isString());
    EXPECT_TRUE(value.isObject());

    EXPECT_THROW({ const auto nullValue = (nullptr_t)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto boolValue = (bool)value; }, std::bad_variant_access);
    EXPECT_THROW({ const auto doubleValue = (double)value; }, std::bad_variant_access);
    EXPECT_NO_THROW({ const auto stringValue = (std::string)value; });
    EXPECT_NO_THROW({ const auto objectValue = (ObjectPtr)value; });

    {
        const auto object2 = (ObjectPtr)value;
        // expect 4 references, one "function", one "object", one boxed in "value" and "object2"
        EXPECT_EQ(object2.use_count(), 4);
    }
    // "object2" is gone
    EXPECT_EQ(object.use_count(), 3);
    EXPECT_EQ((std::string)value, "<func testFunction(#1)>");
}