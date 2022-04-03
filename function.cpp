#include "function.h"

std::string Function::toString() const
{
    std::ostringstream ss;
    ss << "<func " << name << "(#" << arity << ")>";
    return ss.str();
}

bool Function::isFalsey() const
{
    return true;
}

bool Function::isTruthy() const
{
    return false;
}

bool Function::isEqual(const Object &obj) const
{
    if (typeid(obj) == typeid(*this))
    {
        auto &other = reinterpret_cast<const Function &>(obj);
        return isEqual(other);
    }
    return false;
}

bool Function::isEqual(const Function &other) const
{
    // TODO: implement equality for chunks
    return arity == other.arity && name == other.name;
}

bool operator==(const Function &lhs, const Function &rhs)
{
    return lhs.isEqual(rhs);
}