#ifndef __cpplox_value_h
#define __cpplox_value_h

#include "common.h"
#include "object.h"

union uint32bytes
{
    uint32_t u32;
    struct _bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } bytes;
};

typedef std::variant<nullptr_t, bool, double, std::string, Object> BaseValue;

class Value : public BaseValue
{
public:
    explicit Value() {}
    explicit Value(nullptr_t value) : BaseValue(value) {}
    explicit Value(bool value) : BaseValue(value) {}
    explicit Value(double value) : BaseValue(value) {}
    explicit Value(const char *value) : BaseValue(std::string(value)) {}
    explicit Value(const std::string &value) : BaseValue(value) {}
    explicit Value(const Object &value) : BaseValue(value) {}

    Value(const Value &value) : BaseValue(value) {}
    Value(Value &&value) : BaseValue(value) {}

    inline bool isNull() const { return std::holds_alternative<nullptr_t>(*this); }
    inline bool isBool() const { return std::holds_alternative<bool>(*this); }
    inline bool isDouble() const { return std::holds_alternative<double>(*this); }
    inline bool isString() const { return std::holds_alternative<std::string>(*this); }
    inline bool isObject() const { return std::holds_alternative<Object>(*this); }
    bool isFalsey() const;
    bool isTruthy() const;

    Value &operator=(const Value &value)
    {
        BaseValue::operator=(value);
        return *this;
    }

    operator nullptr_t() const;
    operator bool() const;
    operator double() const;
    operator std::string() const;
    operator Object() const;
};

#endif // __cpplox_value_h