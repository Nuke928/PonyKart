#include "Lua/LuaStackObject.h"

using namespace Ponykart;
using namespace Ponykart::Lua;

LuaStackObject::LuaStackObject(bool value)
{
    data.bVal = value;
}

LuaStackObject::LuaStackObject(const char *value)
{
    data.sVal = value;
}

LuaStackObject::LuaStackObject(int value)
{
    data.iVal = value;
}

LuaStackObject::LuaStackObject(unsigned int value)
{
    data.uiVal = value;
}

bool LuaStackObject::getBoolean() const
{
    return data.bVal;
}

const char *LuaStackObject::getString() const
{
    return data.sVal;
}

int LuaStackObject::getInt() const
{
    return data.iVal;
}

unsigned int LuaStackObject::getUInt() const
{
    return data.uiVal;
}
