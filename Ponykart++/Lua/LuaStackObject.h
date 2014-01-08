#ifndef LUASTACKOBJECT_INCLUDED
#define LUASTACKOBJECT_INCLUDED

namespace Ponykart
{

namespace Lua
{

enum LuaVarType
{
    LUAVARTYPE_BOOLEAN,
    LUAVARTYPE_STRING,
    LUAVARTYPE_INT,
    LUAVARTYPE_UINT,
};

class LuaStackObject
{
public:
    LuaVarType type;

    explicit LuaStackObject(bool value);
    explicit LuaStackObject(const char *value);
    explicit LuaStackObject(int value);
    explicit LuaStackObject(unsigned int value);

    bool getBoolean() const;
    const char *getString() const;
    int getInt() const;
    unsigned int getUInt() const;

private:
    union
    {
        bool bVal;
        const char *sVal;
        int iVal;
        unsigned int uiVal;
    } data;
};

}

}

#endif // LUASTACKOBJECT_INCLUDED
