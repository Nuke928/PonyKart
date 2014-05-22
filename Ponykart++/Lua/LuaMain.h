#ifndef LUAMAIN_H_INCLUDED
#define LUAMAIN_H_INCLUDED

#include <vector>
#include <functional>
#include <mutex>
#include "Kernel/LKernelObject.h"

struct lua_State;

namespace Ponykart
{

namespace Actors{class LThing;}

namespace Lua
{

using LuaEvent = std::vector<std::function<void ()>>;

class LuaMain : public LKernel::LKernelObject
{
public:
	explicit LuaMain();

	void registerLuaFunctions(std::function<void ()> func);
	void runRegisterEvent();

	void doFunctionForLThing(std::string& functionName, Actors::LThing* thing);

    void doFile(const std::string &filename);
    void doString(const std::string &s);

    void quit();
    void restart();

    void print(const std::string &s);

	void loadScriptFiles(const std::string& levelName); ///< Loads up all of the script files. We will also load files from /level scripts/levelName/ if it exists
private:
    lua_State *lua;
    std::mutex luaMutex;

    LuaEvent onRegister;

    void initLuaState();

public:
	static const std::string luaLevelFileLocation;
};

} // Lua
} // Ponykart

#endif // LUAMAIN_H_INCLUDED
