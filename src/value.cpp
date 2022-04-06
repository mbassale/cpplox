#include "value.h"

std::ostream &operator<<(std::ostream &stream, const Value &value)
{
    stream << (std::string)value;
    return stream;
}

bool Value::isFalsey() const
{
    if (isNull())
        return true;
    if (isBool())
        return !(bool)(*this);
    // Only 0 is falsey
    if (isDouble())
        return (double)(*this) == 0.0;
    // Empty string is falsey
    if (isString())
        return ((std::string)(*this))
                   .length() == 0;
    if (isObject())
        return ((ObjectPtr)(*this))->isFalsey();

    throw std::runtime_error("unreachable code.");
}

bool Value::isTruthy() const
{
    if (isNull())
        return false;
    if (isBool())
        return (bool)(*this);
    // Any number != 0 is Truthy
    if (isDouble())
        return (double)(*this) != 0.0;
    // A non-empty string is truthy
    if (isString())
        return ((std::string)(*this))
                   .length() > 0;
    if (isObject())
        return ((ObjectPtr)(*this))->isTruthy();

    throw std::runtime_error("unreachable code.");
}

Value::operator nullptr_t() const
{
    return std::get<nullptr_t>(*this);
}

Value::operator bool() const
{
    return std::get<bool>(*this);
}

Value::operator double() const
{
    return std::get<double>(*this);
}

Value::operator std::string() const
{
    std::ostringstream ss;
    if (std::holds_alternative<nullptr_t>(*this))
    {
        ss << "<nil>";
    }
    else if (std::holds_alternative<bool>(*this))
    {
        ss << (std::get<bool>(*this) ? "true" : "false");
    }
    else if (std::holds_alternative<double>(*this))
    {
        ss << std::get<double>(*this);
    }
    else if (std::holds_alternative<std::string>(*this))
    {
        return std::get<std::string>(*this);
    }
    else if (std::holds_alternative<ObjectPtr>(*this))
    {
        return (std::get<ObjectPtr>(*this))->toString();
    }
    return ss.str();
}

Value::operator ObjectPtr() const
{
    return std::get<ObjectPtr>(*this);
}