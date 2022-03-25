#include "value.h"

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
        ss << std::get<bool>(*this);
    }
    else if (std::holds_alternative<double>(*this))
    {
        ss << std::get<double>(*this);
    }
    else if (std::holds_alternative<std::string>(*this))
    {
        return std::get<std::string>(*this);
    }
    return ss.str();
}
