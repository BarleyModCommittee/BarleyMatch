export module Global;

import index;

/// @brief 是否启用泳池关卡
export bool isPoolEnabled = false;

/// @brief 是否为自动对战模式
export bool isAutoMode = false;

/// @brief 加速倍率
export uint32_t accelerationFactor = 1;

/// @brief 是否绘制场景
export bool shouldDrawBoard = true;

/// @brief 每回合行数
export int row_per_round;

export namespace ChallengeState
{
	typedef ChallengeState Type2;
	/// @brief 无比赛
	inline constexpr Type2 BARLEYMATCH_IDLE = (Type2)65536;
	/// @brief 赛前准备阶段
	inline constexpr Type2 BARLEYMATCH_PREMATCH = (Type2)65537;
	/// @brief 比赛进行中
	inline constexpr Type2 BARLEYMATCH_INMATCH = (Type2)65538;
	/// @brief 赛后阶段
	inline constexpr Type2 BARLEYMATCH_AFTERMATCH = (Type2)65539;
}