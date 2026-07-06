module;

#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <windows.h>

export module LuaEnv;

import index;
import Util;
import Global;

static sol::state lua;
static bool lua_initialized = false;

export void LuaEnvInit();
export void LuaCallSetupRow(int row)
{
	lua["SetupRowPlants"](row);
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

	lua.set_function("CreatePlant", [](int type, int row, int column) {
		return Creator::CreatePlant(static_cast<SeedType::SeedType>(type), row, column);
	});

	std::string config_path = GetWorkingDirName("MatchConfig.lua");
	lua.script_file(config_path);

	sol::protected_function_result result = lua["isPoolEnabled"]();
	if (!result.valid() || result.get_type() != sol::type::boolean)
	{
		sol::error err = result;
		MessageBoxA(NULL, err.what(), "isPoolEnabled 存在异常", MB_ICONERROR);
		ExitProcess(1);
	}
	isPoolEnabled = result.get<bool>();

	sol::protected_function_result result2 = lua["isAutoMode"]();
	if (!result2.valid() || result2.get_type() != sol::type::boolean)
	{
		sol::error err = result2;
		MessageBoxA(NULL, err.what(), "isAutoMode 存在异常", MB_ICONERROR);
		ExitProcess(1);
	}
	isAutoMode = result2.get<bool>();

	lua_initialized = true;
}
