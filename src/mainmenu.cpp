#include "mainmenu.h"
#include "inputs.h"
#include "game.h"

void TickMainMenu(float DeltaTime)
{
	UpdateInputs();
	if (IsKeyReleased(InputKeys::Action))
	{
		CurrentGameMode = GameMode::Mode::InGame;
	}

}