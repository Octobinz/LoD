#pragma once

namespace GameMode
{
	enum Mode
	{
		SplashScreen,
		MainMenu,
		ScrollingIntro,
		InGame
	};
}

void TickGame(float DeltaTime);
extern GameMode::Mode CurrentGameMode;
