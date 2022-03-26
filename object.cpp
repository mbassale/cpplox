#include "object.h"

bool operator==(const Object &lhs, const Object &rhs)
{
    return typeid(lhs) == typeid(rhs) && lhs.isEqual(rhs);
}