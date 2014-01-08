#include "Lua/LuaStack.h"

using namespace Ponykart;
using namespace Ponykart::Lua;

void LuaStack::add(LuaStackObject object)
{
    objects.push_back(object);
}

void LuaStack::pop()
{
    objects.pop_back();
}

unsigned int LuaStack::getSize() const
{
    return objects.size();
}

LuaStackObject& LuaStack::operator[](unsigned int index)
{
    return objects[index];
}

const LuaStackObject& LuaStack::operator[](unsigned int index) const
{
    return objects[index];
}
