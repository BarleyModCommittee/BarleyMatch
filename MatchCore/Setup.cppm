export module Setup;

import index;
import Util;
import Global;

export void SetupEvents();

void onGameSelectorUpdate(PVZ::GameSelector selector)
{
	auto app = PVZ::GetPVZApp();
	app.KillGameSelector();
	app.PreNewGame(PVZLevel::Vasebreaker_Endless, false);
}

void onChallengeInitLevelAfter(PVZ::Challenge challenge)
{
	auto board = PVZ::GetBoard();
	if (board.LevelScene == SceneType::Fog)
	{
		auto lawn = board.GetLawn();
		lawn.SetRouteType(2, RouteType::Land);
		lawn.SetRouteType(3, RouteType::Land);

		for (int col = 0; col < 9; col++)
		{
			lawn.SetGridType(2, col, LawnType::Grass);
			lawn.SetGridType(3, col, LawnType::Grass);
		}
	}
}

void SetupEvents()
{
	PVZ::InitPVZDLL();
	PVZ::Memory::immediateExecute = true;

	Creator::AsmInit();

	if (isAutoMode)
	{
		DisableZombieFailHome();

		EnableBackgroundRunning();
		DisableAllSounds();
		DisableNewParticle();

		DisableMusicInterfaceUpdate();
		DisableMusicUpdate();
		auto music = PVZ::GetMusic();
		music.StopAllMusic();
		music.Disabled = true;
	}

	PVZEvent::GameSelectorUpdateEvent((int)onGameSelectorUpdate);
	ChallengeInitLevelAfterEvent((int)onChallengeInitLevelAfter);
}