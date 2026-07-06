export module MatchCycle;

import index;
import Util;
import Global;
import LuaEnv;
import <fstream>;
import <random>;

export void MatchCycleInit();

std::mt19937_64 RndE;

/// @brief 完成的对局行数
int completed = 0;

/// @brief 总行数
int row_sum;
/// @brief 测试输出结果的标志位。\n
///		1 表示“读入并忽略忽略第 21 列数值”（通常用于 21 列的数值为阵容分的情形）；\n
///		2 表示“额外输出被淘汰时的时间”
int mode_flag;
/// @brief 每场对局的行数
int row_per_round;

/// @brief 赛前准备
void UpdatePreMatch(PVZ::Challenge challenge)
{
	auto board = PVZ::GetBoard();
	board.PlayingTime = 0;
	challenge.AttributeCountdown = 0;
	challenge.ConveyorCountdown = 1;
	challenge.UpgradedRepeater = 0;

	Creator::ResetLawnmowers();
	for (auto proj : board.GetAllProjectiles())
		proj.Remove();

	if (completed == row_sum)
	{
		int zero = 0;
		row_sum = 1 / zero;
	}

	Creator::CreateVase(-3, -3, VaseContent::Sun);

	completed += row_per_round;
	for (int row_index = 0; row_index < row_per_round; row_index++)
		LuaCallSetupRow(row_index);

	challenge.State = ChallengeState::BARLEYMATCH_INMATCH;
}
/// @brief 生成辣椒并刷新倒计时
/// @param rows_cnt 行数上限
void SummonJalapeno(int rows_cnt)
{
	bool hasLanwmower[6] = { false, false, false, false, false, false };
	auto mowers = PVZ::GetBoard().GetAllLawnmowers();
	for (int i = 0, lim = mowers.size(); i < lim; i++)
		if (mowers[i].State == LawnMowerState::Ready)
			hasLanwmower[mowers[i].Row] = true;

	for (int i = 0; i < rows_cnt; i++)
		if (!hasLanwmower[i])
			Creator::CreatePlant(SeedType::Jalapeno, i, 15).DoSpecial();
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

/// @brief 更新辣椒救援倒计时
void UpdateJalapenoRescue()
{
	auto challenge = PVZ::GetBoard().GetChallenge();
	challenge.AttributeCountdown++;
	if (challenge.AttributeCountdown == 1200)
	{
		challenge.AttributeCountdown = 0;
		SummonJalapeno(row_per_round);
	}
}

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
	UpdateJalapenoRescue();
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
		UpdatePreMatch(challenge);
		break;
	case ChallengeState::BARLEYMATCH_INMATCH:
		UpdateInMatch();
	}
}

/// @brief 队伍丢失小推车
void TeamEliminated(PVZ::LawnMower mower)
{
	mower.State = LawnMowerState::Triggered;

	auto challenge = PVZ::GetBoard().GetChallenge();
	challenge.UpgradedRepeater++;

	if (challenge.UpgradedRepeater == row_per_round)
	{
		challenge.State = ChallengeState::BARLEYMATCH_AFTERMATCH;

		auto vases = PVZ::GetBoard().GetAllGriditems<PVZ::Vase>();
		for (auto vase : vases)
			vase.Open();
		PVZ::Memory::Execute(AsmBuilder()
			.push(0)
			.mov_reg_imm(0, 0)
			.mov_reg_imm(1, challenge.GetBaseAddress())
			.invoke(0x429980)
			.ret()
		);
	}
}

/// @brief 对局结束
bool RoundComplete(PVZ::Challenge challenge, int GridX, int GridY)
{
	auto mowers = PVZ::GetBoard().GetAllLawnmowers();
	for (auto mower : mowers)
		if (mower.State != LawnMowerState::Triggered)
		{
			break;
		}

	// for event
	return false;
}

void MatchCycleInit()
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
	PVZEvent::PuzzlePhaseCompleteBonusEvent_ts((int)RoundComplete);
}