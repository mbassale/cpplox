#include "native.h"

std::string NativeFunction::toString() const
{
    std::ostringstream ss;
    ss << "<native " << name << "@" << functionPtr << ">";
    return ss.str();
}

bool NativeFunction::isFalsey() const
{
    return true;
}

bool NativeFunction::isTruthy() const
{
    return false;
}

bool NativeFunction::isEqual(const Object &obj) const
{
    if (typeid(obj) == typeid(*this))
    {
        auto &other = reinterpret_cast<const NativeFunction &>(obj);
        return isEqual(other);
    }
    return false;
}

bool NativeFunction::isEqual(const NativeFunction &other) const
{
    // TODO: implement equality for chunks
    return name == other.name && functionPtr == other.functionPtr;
}

bool operator==(const NativeFunction &lhs, const NativeFunction &rhs)
{
    return lhs.isEqual(rhs);
}