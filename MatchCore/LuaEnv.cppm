module;

#define SOL_LUAJIT 1
#include <sol/sol.hpp>

export module LuaEnv;

import index;
import Util;

export sol::state& GetLuaState();
export void LuaEnvInit();

static sol::state lua;
static bool lua_initialized = false;

sol::state& GetLuaState()
{
	return lua;
}

void LuaEnvInit()
{
	if (lua_initialized)
		return;

	lua.open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::coroutine,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::io
	);

	lua_initialized = true;
}
