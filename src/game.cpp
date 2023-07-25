#include "game.h"
#include "ui.h"
#include "inputs.h"
#include "combat.h"
#include "raycaster.h"
#include "enemies.h"
#include "skills.h"
#include "party.h"

GameMode::Mode CurrentGameMode = GameMode::Mode::MainMenu;

void UpdateNavigation(float DeltaTime)
{
	Lights[0].location.x = Context.Position.x;
	Lights[0].location.y = Context.Position.y;
	Lights[0].radius = 10.0f;
	if (IsKeyReleased(InputKeys::Action)) 
	{
		//pitch += 10;
		//lights[0].radius -= 0.1f;
	}
	const float rotspeed = 3.0f * 0.016f,
	movespeed = 3.0f * 0.016f;

	if (IsKeyDown(InputKeys::Left)) 
	{
		rotate(+rotspeed);
	}

	if (IsKeyDown(InputKeys::Right)) 
	{
		rotate(-rotspeed);
	}


	if (IsKeyDown(InputKeys::Up)) 
	{

		float futurex = Context.Position.x + Context.Direction.x * movespeed;
		float futurey = Context.Position.y + Context.Direction.y * movespeed;
		if(MAPDATA[int(futurex) + int(Context.Position.y) * MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		//if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		if (MAPDATA[int(Context.Position.x) + int(futurey) * MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}

	if (IsKeyDown(InputKeys::Down)) 
	{

		float futurex = Context.Position.x - Context.Direction.x * movespeed;
		float futurey = Context.Position.y - Context.Direction.y * movespeed;

		if(MAPDATA[int(futurex) + int(Context.Position.y)* MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		if (MAPDATA[int(Context.Position.x) + int(futurey)* MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}

	AddEnemiesClosestTo(Context.Position, Context.Radius);

	if(CurrentGameState == GameState::Combat)
	{
		GenerateGameTurns();
		//int Initiative = 0; //GetInitiative();
		GameTurn CurrentGameTurn = GameTurns.Get(CurrentCombatIndex);
		if(CurrentGameTurn.IsPartyMember)
		{
			if (CurrentCombatIndex == 0)
			{
				CurrentPartyContext.CurrentPartyMember = CurrentGameTurn.index;
				char Msg[128];
				sprintf(Msg, "%s has the initiative!", Party[CurrentGameTurn.index].Name);
				QueuePopupMessage(Msg, 2.0f);
			}
			CurrentTurn = CombatTurn::Player;
		}
		else
		{
			CurrentTurn = CombatTurn::Enemies;
			char Msg[128];
			sprintf(Msg, "%s has the initiative!", GameEnemies[CurrentGameTurn.index].Name);
			
			if(CurrentCombatIndex == 0)
				QueuePopupMessage(Msg, 2.0f);
			
			/*const auto processor_count = std::thread::hardware_concurrency();
					sprintf(Msg, "%d core count!", processor_count);
					QueuePopupMessage(Msg, 2.0f);*/
		}
	}
}

void TickGame(float DeltaTime)
{
	if (!EventQueue.empty())
	{
		for(int i = 0; i < EventQueue.size(); i++)
		{
			EventSystem::Event& M = EventQueue[i];
			M.TimeLeft -= DeltaTime;
			if(M.TimeLeft <= 0.0f && M.processed)
			{
				switch (M.EventType)
				{
					case EventSystem::Type::KillEnemy:
					{
						KillEnemyMessage* Msg = (KillEnemyMessage*)M.Data;
						Enemy& CurrentEnemy = GameEnemies[Msg->Target];
						RemoveEnemy(Msg->Target);
						for(u32 j = 0; j < EngagedEnemies.size(); j++)
						{
							if (Msg->Target == EngagedEnemies[j])
							{
								EngagedEnemies.erase(j);
								break;
							}
						}
						//Regenerate game turns as the number of enemies and indices changed
						GenerateGameTurns();

						if (EngagedEnemies.size() == 0)
						{
							CurrentPartyContext.CurrentPartyMember = -1;
							CurrentGameState = GameState::Navigation;
						}
					}
					break;
					case EventSystem::Type::PlayerHurt:
					{
						ApplyDamageToPartyMember* DmgBundle = (ApplyDamageToPartyMember*)M.Data;
						DmgBundle->TargetPartyMember->HP -= DmgBundle->DamageAmount;
					}
					break;
					case EventSystem::Type::EnemyHurt:
					{
						if (pd->sound->fileplayer->loadIntoPlayer(sfxplayer, "sfx/sword_woosh"))
						{
							pd->sound->fileplayer->play(sfxplayer, 1);
						}

						ApplyDamageToEnemy* DmgBundle = (ApplyDamageToEnemy*)M.Data;
						
						Enemy& CurrentEnemy = GameEnemies[DmgBundle->TargetEnemy];
						CurrentEnemy.HP -= DmgBundle->DamageAmount;
						if (CurrentEnemy.HP > 0)
						{
							Sprite& CurrentSprite = Sprites[GameObjects[CurrentEnemy.IdleObject].ObjectSprite];
							initSpriteShake(&CurrentSprite, 1, 2); // Initialize the sprite shake with a duration of 5 seconds
						}
						else
						{
							if(CurrentEnemy.HP <= 0)
							{
								QueueKillEnemy(DmgBundle->TargetEnemy);
							}
						}

					}
					break;
				}

				delete M.Data;
				EventQueue.erase(EventQueue.begin() + i);
				return;
			}
			else
			{
				if(M.blocking)
				{
					return;
				}
			}
		}
	}

	UpdateInputs();

	switch(CurrentGameState)
	{
		case GameState::Combat:
			UpdateCombat(DeltaTime);
			break;
		case GameState::Navigation:
			UpdateNavigation(DeltaTime);
			break;
	}
}