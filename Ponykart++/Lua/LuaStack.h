#ifndef LUASTACK_H_INCLUDED
#define LUASTACK_H_INCLUDED

#include "Lua/LuaStackObject.h"

#include <vector>

namespace Ponykart
{

namespace Lua
{

/**
This class represents a stack for Lua to be pushed or popped.
*/
class LuaStack
{
public:
    void add(LuaStackObject object);
    void pop();

    unsigned int getSize() const;

    LuaStackObject &operator[](unsigned int index);
    const LuaStackObject &operator[](unsigned int index) const;

private:
    std::vector<LuaStackObject> objects;
};

}

}

#endif // LUASTACK_H_INCLUDED
