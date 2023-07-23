#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <vector>
#include <tchar.h>
#include <algorithm>

extern "C"
{
#include "pd_api.h"
}
#include "geometry.h"
#include "gameobject.h"
#include "raycaster.h"
#include "locator.h"
#include "inputs.h"
#include "growarray.h"
#include "enemies.h"
#include "party.h"
#include "ui.h"
#include "skills.h"

static int update(void* userdata);

unsigned int StartupMilliseconds = 0;
float CurrentDeltaMilliseconds = 0.0f;
unsigned int LastMilliseconds = 0;
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue>

enum CombatTurn
{
	Player,
	Enemies
};


CombatTurn CurrentTurn = Player;
u32 CurrentEnemyTurn = 0;

void RenderFrame() 
{
	if (CurrentGameState != GameState::State::Combat)
	{
		renderFloor();
		renderWalls();
	}

	if (CurrentGameState == GameState::State::Combat)
	{
		RenderCombatBG();
	}
	renderSprites();
}

struct GameTurn
{
	bool IsPartyMember = false;
	float Initiative = 0.0f;
	int index = -1;
};

int CurrentCombatIndex = 0;
GrowArray<GameTurn> GameTurns;

void GenerateGameTurns()
{
	GameTurns.Reset();

	for(u32 i = 0; i < EngagedEnemiesCount; i++)
	{
		Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[i]];
		GameTurn NewGameTurn;
		if (CurrentEnemy.Engaged && IsIndexValid(EnemiesBundle, EngagedEnemies[i]))
		{
			NewGameTurn.IsPartyMember = false;
			NewGameTurn.index = EngagedEnemies[i];
			NewGameTurn.Initiative = CurrentEnemy.Initiative;
			GameTurns.AddElement(NewGameTurn);
		} 
	}

	for(u32 i = 0; i < CurrentPartyCount; i++)
	{
		PartyMember& CurrentPartyMember = Party[i];
		GameTurn NewGameTurn;

		if (IsIndexValid(PartyBundle, i))
		{
			NewGameTurn.IsPartyMember = true;
			NewGameTurn.index = i;
			NewGameTurn.Initiative = CurrentPartyMember.Initiative;
			GameTurns.AddElement(NewGameTurn);
		} 
	}
	
	//Sort game turns by Initiative
	for(u32 i = 0; i < GameTurns.size(); i++)
	{
		for(u32 j = 0; j < GameTurns.size(); j++)
		{
			if(GameTurns[i].Initiative > GameTurns[j].Initiative)
			{
				GameTurn NewGameTurn;
				std::swap(GameTurns[i], GameTurns[j]);
			}
		}
	}
}

static LCDBitmap *loadImageAtPath(const char *path)
{
	const char *outErr = nullptr;
	LCDBitmap *img = pd->graphics->loadBitmap(path, &outErr);
	if ( outErr != nullptr ) 
	{
		pd->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
	}
	return img;
}

void RemoveEnemy(u32 index)
{
	Enemy& CurrentEnemy = GameEnemies[index];
	RemoveSprite(GameObjects[CurrentEnemy.IdleObject].ObjectSprite);
	ReleaseIndex(GameObjectsBundle, CurrentEnemy.IdleObject);
	ReleaseIndex(EnemiesLocatorsBundle, CurrentEnemy.Locator);
	ReleaseIndex(EnemiesBundle, index);
}

void AddEnemy(const char* EnemyName, float Initiative, float x, float y)
{
	u32 LocatorIndex = GetNextIndex(EnemiesLocatorsBundle);
	u32 IdleObjectIndex = GetNextIndex(GameObjectsBundle);
	u32 EnemyIndex = GetNextIndex(EnemiesBundle);

	Enemy& CurrentEnemy = GameEnemies[EnemyIndex];
	strcpy_s(CurrentEnemy.EnemyName, 128, EnemyName);
	CurrentEnemy.IdleObject = IdleObjectIndex;
	CurrentEnemy.Locator = LocatorIndex;
	GameObjects[IdleObjectIndex].ObjectSprite = AddSprite("textures/skeleton.png", x, y);
	EnemiesLocations[LocatorIndex].x = x;
	EnemiesLocations[LocatorIndex].y = y;
}

void InitPlayer()
{
	memset(EngagedEnemies, -1, MaxEnemies*sizeof(i32));
	CurrentGameState = GameState::Navigation;
	Context.SwordSprite.texture = LoadTexture("textures/player_sword_1.png");
	Context.ShieldSprite.texture = LoadTexture("textures/player_shield_1.png");
	Context._64SquareSprite.texture = LoadTexture("textures/ui/64Square.png");
	Context.SlashSprite.texture = LoadTexture("textures/ui/slash_effect.png");
	Context._256RectangleSprite.texture = LoadTexture("textures/ui/256Rectangle.png");

	//Create party
	AddPartyMember("Player", "textures/ui/warrior_mugshot.png", 1.0f);
	AddPartyMember("Monk", "textures/ui/monk_mugshot.png", 2.0f);
	AddPartyMember("Witch", "textures/ui/witch_mugshot.png", 0.5f);
	AddPartyMember("Otter", "textures/ui/otter_mugshot.png", 7.0f);
	
	//QueueDialogueMessage("I think I sense a trap", 5.0f, true);
}



bool ProcessMenuInputs(float DeltaTime, u32& InOutOption, int ElementsCount)
{
	u32 CurrentSelection = (u32)InOutOption;
	if (IsKeyReleased(InputKeys::Right))
	{
		CurrentSelection += 2;
	}
	if (IsKeyReleased(InputKeys::Left))
	{
		CurrentSelection -= 2;
	}
	if (IsKeyReleased(InputKeys::Up))
	{
		CurrentSelection--;
	}
	if (IsKeyReleased(InputKeys::Down))
	{
		CurrentSelection++;
	}


	InOutOption = std::max<u32>(std::min<u32>(CurrentSelection, ElementsCount-1), 0);

	if (IsKeyReleased(InputKeys::Action)) 
	{
		return true;
	}
	if (IsKeyReleased(InputKeys::Action2)) 
	{
		Context.CurrentCombatMenu = SelectedMenu::Menu::Combat;
		//Context.CurrentCombatMenu = SelectedMenu::Menu::Attack;
	}
	return false;
}

//true if attacked
bool UpdateCombatMenu(float DeltaTime)
{
	bool bAttacked = false;
	switch(Context.CurrentCombatMenu)
	{
		case SelectedMenu::Menu::Combat:
		{
			u32 Option = (u32)Context.CurrentCombatOption;
			if(ProcessMenuInputs(DeltaTime,Option,3))
			{
				switch(Option)
				{
					case 0:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Attack;
						break;
					case 1:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Spell;
						break;
					case 2:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Item;
						break;
				}
			}
			Context.CurrentCombatOption = (CombatMenu::Option)Option;
		} break;
		case SelectedMenu::Menu::Attack:
		{
			u32 Option = (u32)Context.CurrentAttackOption;
			bAttacked = ProcessMenuInputs(DeltaTime, Option, 3);
			Context.CurrentAttackOption = (CombatMenu::AttackOption)Option;
		} break;
		case SelectedMenu::Menu::Spell:
		{
			u32 Option = (u32)Context.CurrentSpellOption;
			bAttacked = ProcessMenuInputs(DeltaTime, Option, 2);
			Context.CurrentSpellOption = (CombatMenu::SpellOption)Option;
		} break;
	}

	return bAttacked;
}

/*
	//Make enemies that should attack attack
	//Check for player attack and damage	
*/
void UpdateCombat(float DeltaTime)
{

	if(Context.WaitForButtonRelease == true)
	{
		if(inputs_down != 0)
			return;
		Context.WaitForButtonRelease = false;
		inputs_released = 0;
	}

	GameTurn CurrentGameTurn = GameTurns.Get(CurrentCombatIndex);
	CurrentTurn = CurrentGameTurn.IsPartyMember ? CombatTurn::Player : CombatTurn::Enemies;

	bool bAttacked = UpdateCombatMenu(DeltaTime);
	
	if (CurrentTurn == CombatTurn::Player)
	{
		CurrentPartyContext.CurrentPartyMember = CurrentGameTurn.index;

		if (bAttacked)
		{
			int EnemyIndex = 0;
			Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[EnemyIndex]];
			if (CurrentEnemy.Engaged && IsIndexValid(EnemiesBundle, EngagedEnemies[EnemyIndex]))
			{
				GameSkill stab = Stab;
				stab.PlayerAction(CurrentGameTurn.index, CurrentEnemy);
			}

			if (CurrentEnemy.HP <= 0)
			{
				RemoveEnemy(EngagedEnemies[EnemyIndex]);
				EngagedEnemies[EnemyIndex] = -1;
				memcpy(&EngagedEnemies[EnemyIndex], &EngagedEnemies[EnemyIndex + 1], (MaxEnemies - EnemyIndex)-1);
				EngagedEnemiesCount--;

				//Regenerate game turns as the number of enemies and indices changed
				GenerateGameTurns();

				if (EngagedEnemiesCount == 0)
				{
					CurrentPartyContext.CurrentPartyMember = -1;
					CurrentGameState = GameState::Navigation;
				}
			}
			
			CurrentCombatIndex++;
			if(CurrentCombatIndex >= GameTurns.size())
			{
				CurrentCombatIndex = 0;
			}
		}
	}
	else
	{
		char Msg[128];
		sprintf(Msg, "%s uses it's sword!", GameEnemies[CurrentGameTurn.index].EnemyName);
		QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
		QueuePopupMessage(Msg, 2.0f);
		QueuePopupMessage("15 Damages received!", 1.5f);

		CurrentCombatIndex++;
		if(CurrentCombatIndex > GameTurns.size())
		{
			CurrentCombatIndex = 0;
		}
	}

}

void AddEnemiesClosestTo(vector2& InLocation, float Radius)
{
	for(u32 i = 0; i < EnemiesBundle.MaxIndex; i++)
	{
		vector2& CurrentLocation = EnemiesLocations[i];
		vector2 DistanceVector;
		vector_sub(DistanceVector, CurrentLocation, InLocation);
		float DP = vector_dotproduct(DistanceVector, DistanceVector);

		if (DP < (Radius*Radius) && !GameEnemies[i].Engaged && IsIndexValid(EnemiesBundle, i))
		{
			GameEnemies[i].Engaged = true;
			EngagedEnemies[EngagedEnemiesCount] = i;
			++EngagedEnemiesCount;
			CurrentGameState = GameState::Combat;
			Context.WaitForButtonRelease = true;
			AddEnemiesClosestTo(CurrentLocation, 1.5f);
		}
	}
}

void UpdateNavigation(float DeltaTime)
{
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

	EngagedEnemiesCount = 0;
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
			sprintf(Msg, "%s has the initiative!", GameEnemies[CurrentGameTurn.index].EnemyName);
			
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

#ifdef _WINDLL
__declspec(dllexport)
#endif
	int eventHandler(PlaydateAPI* InPD, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		pd = InPD;
		uint8_t* pd_screen = pd->graphics->getFrame();
		screen_fb = pd_screen;
		
		//Load fonts		
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		fontSmall = pd->graphics->loadFont(fontpathSmall, &err);

		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->setUpdateCallback(update, pd);
		pd->display->setRefreshRate(50);
		pd->display->setScale(1);		

		LoadTexture( "textures/eagle.png");
		LoadTexture( "textures/blank_rectangle.png");
		LoadTexture( "textures/purplestone.png");
		LoadTexture( "textures/greystone.png");
		LoadTexture( "textures/bluestone.png");
		LoadTexture( "textures/mossy.png");
		LoadTexture( "textures/wood.png");
		LoadTexture( "textures/colorstone.png");
		LoadTexture( "textures/skeleton.png");
		LoadTexture( "textures/pillar.png");
		LoadTexture( "textures/greenlight.png");
		LoadTexture( "textures/woodenfloor.png");
		LoadTexture( "textures/greystone.png");
		LoadTexture( "textures/skeleton.png");

		//memset(EngagedEnemies, -1, sizeof(Enemy)*MaxEnemies);
		InitInputs();
		InitPlayer();
		AddEnemy("Skeleton", 1.0f, 21.5f, 1.5f);
		AddEnemy("Skeleton", 1.0f, 15.5f, 1.5f);
		AddEnemy("Skeleton", 1.0f, 16.0f, 1.8f);
		AddEnemy("Skeleton", 1.0f, 16.2f, 1.2f);
		AddEnemy("Skeleton", 1.0f, 3.5f,  2.5f);
		AddEnemy("Skeleton", 1.0f, 9.5f, 15.5f);
		AddEnemy("Skeleton", 1.0f, 10.0f, 15.1f);
		AddEnemy("Skeleton", 1.0f, 10.5f, 15.8f);

		AddLight(1.0f, 1.0f, 20.0f);
		AddLight(8.0f, 8.0f, 2.5f);

		Context.Position.x = 5; 
		Context.Position.y = 5;

		vector2 v = {-1.0f, 0.1f};
		vector_normalize(v);
		Context.Direction = v;
		Context.Plane = { 0.0f, 0.66f };
		StartupMilliseconds = pd->system->getCurrentTimeMilliseconds();
		CurrentDeltaMilliseconds = 0;
		LastMilliseconds = StartupMilliseconds;

	}
	
	return 0;
}

static int offset = 0;
static int update(void* userdata)
{
	pd = (PlaydateAPI *)userdata;
	unsigned int MS;
	MS = pd->system->getCurrentTimeMilliseconds();
	CurrentDeltaMilliseconds = (MS - LastMilliseconds) / 1000.0f;
	LastMilliseconds = MS;

	pd->graphics->markUpdatedRows(0, 240 - 1);
	//pd->graphics->clear(kColorWhite);

	TickGame(CurrentDeltaMilliseconds);
	RenderFrame();

	GameTexture& SwordTexture = texture.Get(Context.SwordSprite.texture);
	pd->graphics->drawBitmap(SwordTexture.img, 250, 0, kBitmapUnflipped);
	pd->system->drawFPS(0, 0);

	renderUI(CurrentDeltaMilliseconds);

	return 1;
}

