#ifndef __cpplox_value_h
#define __cpplox_value_h

#include "common.h"

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

class Value : public std::variant<nullptr_t, bool, double>
{
public:
    Value() {}
    Value(double value) : std::variant<nullptr_t, bool, double>(value) {}
    Value(Value &value) : std::variant<nullptr_t, bool, double>(value) {}
    Value(Value &&value) : std::variant<nullptr_t, bool, double>(value) {}

    inline bool isDouble() { return std::holds_alternative<double>(*this); }
    inline bool isBool() { return std::holds_alternative<bool>(*this); }
    inline bool isNull() { return std::holds_alternative<nullptr_t>(*this); }

    Value operator=(const Value &value)
    {
        std::variant<nullptr_t, bool, double>::operator=(value);
        return *this;
    }
    operator std::string() const;
    operator double() const;
    operator bool() const;
    operator nullptr_t() const;
};

#endif // __cpplox_value_h