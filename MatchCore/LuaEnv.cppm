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
export void LuaCallOnMatchInit()
{
	lua["OnMatchInit"]();
}
export void LuaCallOnPreMatch()
{
	lua["OnPreMatch"]();
}
export void LuaCallOnMatchUpdate()
{
	lua["OnMatchUpdate"]();
}
export void LuaCallOnTeamEliminated(int row)
{
	lua["OnTeamEliminated"](row);
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

	lua.set_function("CreateZombie", [](int type, int row, int column, float x) {
		auto zombie = Creator::CreateZombie(static_cast<ZombieType::ZombieType>(type), row, column);
		zombie.X = x;
		return zombie;
	});

	lua.set_function("ClearPlants", []() {
		for (auto plant : PVZ::GetBoard().GetAllPlants())
			plant.Remove();
	});

	lua.set_function("Terminate", []() {
		auto challenge = PVZ::GetBoard().GetChallenge();
		challenge.State = ChallengeState::BARLEYMATCH_AFTERMATCH;
		challenge.AttributeCountdown = 1;
		lua["OnTerminate"]();
	});

	struct MatchProxy {};
	auto ut = lua.new_usertype<MatchProxy>("Match", sol::no_constructor);
	ut["StateCountdown"] = sol::property(
		[](MatchProxy&) -> int {
			return PVZ::GetBoard().GetChallenge().AttributeCountdown;
		},
		[](MatchProxy&, int v) {
			PVZ::GetBoard().GetChallenge().AttributeCountdown = v;
		}
	);
	ut["PrimaryCounter"] = sol::property(
		[](MatchProxy&) -> int {
			return PVZ::GetBoard().PlayingTime;
		},
		[](MatchProxy&, int v) {
			PVZ::GetBoard().PlayingTime = v;
		}
	);
	ut["SecondaryCounter"] = sol::property(
		[](MatchProxy&) -> int {
			return PVZ::GetBoard().GetChallenge().ConveyorCountdown;
		},
		[](MatchProxy&, int v) {
			PVZ::GetBoard().GetChallenge().ConveyorCountdown = v;
		}
	);
	ut["TertiaryCounter"] = sol::property(
		[](MatchProxy&) -> int {
			return PVZ::GetBoard().GetChallenge().LevelProcess;
		},
		[](MatchProxy&, int v) {
			PVZ::GetBoard().GetChallenge().LevelProcess = v;
		}
	);
	ut["Round"] = sol::property(
		[](MatchProxy&) -> int {
			return PVZ::GetBoard().GetChallenge().Round;
		},
		[](MatchProxy&, int v) {
			PVZ::GetBoard().GetChallenge().Round = v;
		}
	);
	ut["RowsPerRound"] = sol::property(
		[](MatchProxy&) -> int {
			return row_per_round;
		}
	);
	lua["Match"] = MatchProxy{};

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
