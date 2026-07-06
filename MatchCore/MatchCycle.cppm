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

int time_lim[] = { 6000, 12000, 18000, 24000, 30000, 36000, 42000, 60000, 66000, 72000,
	78000, 84000, 90000, 96000, 102000, 108000, 114000, 120000, 126000, 132000,
	138000, 144000, 150000, 156000, 162000, 168000, 172000, 180000, 186000, 210000,
	216000, 222000, 228000, 234000, 0x3F3F3F3F };
int interval_next[] = { 1200, 1100, 1050, 1000, 950,  900,  850,   800,   750,   650,  
	  550,   440,   400,	360,	340,	330,	 320,	 310,   300,	290,
	  280,	270,	260,	240,	210,	180,	 150,	 120,	 100,	100,
	  70, 65, 60, 55, 50 };
int maxtier = sizeof(time_lim) / sizeof(int);

/// @brief 重置僵尸刷新倒计时
void ResetZombieTimer()
{
	int timing = PVZ::GetBoard().PlayingTime;
	int i = 0;

	for (; i < maxtier; i++)
		if (time_lim[i] >= timing)
			break;

	auto challenge = PVZ::GetBoard().GetChallenge();
	challenge.ConveyorCountdown = interval_next[i];
}

static int zombie_rows[6] = { 0, 1, 2, 3, 4, 5 };

/// @brief 更新僵尸生成倒计时
void UpdateZombieSpawn()
{
	auto challenge = PVZ::GetBoard().GetChallenge();
	challenge.ConveyorCountdown--;
	if (challenge.ConveyorCountdown == 0)
	{
		ResetZombieTimer();

		std::shuffle(zombie_rows, zombie_rows + row_per_round, RndE);

		for (int i = 0; i < row_per_round; i++)
		{
			auto zombie = Creator::CreateZombie(ZombieType::ConeheadZombie, zombie_rows[i], 10);
			zombie.X = Creator::RandFloat(40) + 780.0f;
		}
	}
}

/// @brief 对战中状态更新
void UpdateInMatch()
{
	auto challenge = PVZ::GetBoard().GetChallenge();
	if (challenge.AttributeCountdown > 0)
		challenge.AttributeCountdown--;
	UpdateZombieSpawn();
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