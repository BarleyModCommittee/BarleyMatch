export module MatchCycle;

import index;
import Util;
import Global;
import LuaEnv;
import <fstream>;
import <random>;

std::mt19937_64 RndE;

/// @brief 开始赛前准备
export void MatchStart()
{
	auto board = PVZ::GetBoard();
	auto challenge = board.GetChallenge();

	board.PlayingTime = 0;
	challenge.LevelProcess = 1;
	challenge.ConveyorCountdown = 1;

	Creator::ResetLawnmowers();
	for (auto proj : board.GetAllProjectiles())
		proj.Remove();
	for (auto plant : board.GetAllPlants())
		plant.Remove();
	for (auto zombie : board.GetAllZombies())
		zombie.Remove();
	for (auto coin : board.GetAllCoins())
		coin.Die();
	for (auto griditem : board.GetAllGriditems())
	{
		if (griditem.Row == -3 && griditem.Column == -3 && griditem.Type == GriditemType::Vase)
			continue;
		griditem.Remove();
	}

	challenge.AttributeCountdown = 1;
	challenge.State = ChallengeState::BARLEYMATCH_PREMATCH;

	LuaCallOnMatchInit();
}

/// @brief 对战中状态更新
void UpdateInMatch()
{
	auto challenge = PVZ::GetBoard().GetChallenge();
	if (challenge.LevelProcess > 0)
		challenge.LevelProcess--;
	if (challenge.ConveyorCountdown > 0)
		challenge.ConveyorCountdown--;

	LuaCallOnMatchUpdate();
}

/// @brief 对局更新
void RoundUpdate(PVZ::Board board)
{
	board.FogBlownCountDown = 10000;

	if (PVZ::GetPVZApp().GameState != PVZGameState::Playing)
		return;

	auto challenge = PVZ::GetBoard().GetChallenge();

	switch (challenge.State)
	{
	case ChallengeState::BARLEYMATCH_IDLE:
		if (isAutoMode)
			MatchStart();
		break;
	case ChallengeState::BARLEYMATCH_PREMATCH:
	{
		LuaCallOnPreMatch();
		if (challenge.AttributeCountdown > 0)
		{
			challenge.AttributeCountdown--;
			if (challenge.AttributeCountdown == 0)
				challenge.State = ChallengeState::BARLEYMATCH_INMATCH;
		}
	}
		break;
	case ChallengeState::BARLEYMATCH_INMATCH:
		UpdateInMatch();
		break;
	case ChallengeState::BARLEYMATCH_AFTERMATCH:
		if (challenge.AttributeCountdown > 0)
		{
			challenge.AttributeCountdown--;
			if (challenge.AttributeCountdown == 0)
				challenge.State = ChallengeState::BARLEYMATCH_IDLE;
		}
		break;
	}
}

/// @brief 队伍丢失小推车
void TeamEliminated(PVZ::LawnMower mower)
{
	mower.State = LawnMowerState::Triggered;
	LuaCallOnTeamEliminated(mower.Row);
}

export void MatchCycleInit()
{
	if (isPoolEnabled)
	{
		row_per_round = 6;
		Const::SetLevelScene(PVZLevel::Vasebreaker_Endless, SceneType::Fog);
	}
	else
		row_per_round = 5;

	std::random_device device;
	RndE = std::mt19937_64(device());

	BoardUpdateGameEvent((int)RoundUpdate);
	LawnmowerStartEvent((int)TeamEliminated);
}