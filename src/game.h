#pragma once

namespace GameMode
{
	enum Mode
	{
		SplashScreen,
		MainMenu,
		InGame
	};
}

void TickGame(float DeltaTime);
extern GameMode::Mode CurrentGameMode;
