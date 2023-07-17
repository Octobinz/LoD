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

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;
unsigned int StartupMilliseconds = 0;
float CurrentDeltaMilliseconds = 0.0f;
unsigned int LastMilliseconds = 0;
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void render() 
{
	renderFloor();
	renderWalls();
	renderSprites();
}

void renderUI()
{
	if (CurrentGameState == GameState::Combat)
	{
		GameTexture& UI64Square = texture[Context._64SquareSprite.texture];
		pd->graphics->drawBitmap(UI64Square.img, 0, 175, kBitmapUnflipped);
	
		GameTexture& UI256Rectangle = texture[Context._256RectangleSprite.texture];
		pd->graphics->drawBitmap(UI256Rectangle.img, 70, 175, kBitmapUnflipped);
		
		switch(Context.CurrentCombatOption)
		{
			case CombatMenu::Option::Attack:
			{
				pd->graphics->fillRect(77, 183, 110, 20,  kColorBlack);
				pd->graphics->setDrawMode( kDrawModeInverted );
				pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
				pd->graphics->setDrawMode( kDrawModeCopy );
				pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
				pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
			}
			break;
			case CombatMenu::Option::Spell:
			{
				pd->graphics->fillRect(77, 213, 110, 20,  kColorBlack);
				pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
				pd->graphics->setDrawMode( kDrawModeInverted );
				pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
				pd->graphics->setDrawMode( kDrawModeCopy );
				pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
			}
			break;
			case CombatMenu::Option::Object:
			{
				pd->graphics->fillRect(197, 183, 110, 20,  kColorBlack);
				pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
				pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
				pd->graphics->setDrawMode( kDrawModeInverted );
				pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
				pd->graphics->setDrawMode( kDrawModeCopy );
			}
			break;
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
	RemoveSprite(GameObjects[CurrentEnemy.Object].ObjectSprite);
	ReleaseIndex(GameObjectsBundle, CurrentEnemy.Object);
	ReleaseIndex(EnemiesLocatorsBundle, CurrentEnemy.Locator);
	ReleaseIndex(EnemiesBundle, index);
}

void AddEnemy(float x, float y)
{
	u32 LocatorIndex = GetNextIndex(EnemiesLocatorsBundle);
	u32 GameObjectIndex = GetNextIndex(GameObjectsBundle);
	u32 EnemyIndex = GetNextIndex(EnemiesBundle);

	Enemy& CurrentEnemy = GameEnemies[EnemyIndex];
	CurrentEnemy.Object = GameObjectIndex;
	CurrentEnemy.Locator = LocatorIndex;
	GameObjects[GameObjectIndex].ObjectSprite = AddSprite("textures/skeleton.png", x, y);
	EnemiesLocations[LocatorIndex].x = x;
	EnemiesLocations[LocatorIndex].y = y;
}

void InitPlayer()
{
	Context.Radius = 1.1f;
	CurrentGameState = GameState::Navigation;
	Context.SwordSprite.texture = LoadTexture("textures/player_sword_1.png");
	Context.ShieldSprite.texture = LoadTexture("textures/player_shield_1.png");
	Context._64SquareSprite.texture = LoadTexture("textures/ui/64Square.png");
	Context._256RectangleSprite.texture = LoadTexture("textures/ui/256Rectangle.png");
}



/*
	//Make enemies that should attack attack
	//Check for player attack and damage	
*/
void UpdateCombat(float DeltaTime)
{
	bool bAttacked = false;

	if(Context.WaitForButtonRelease == true)
	{
		if(inputs_down != 0)
			return;
		Context.WaitForButtonRelease = false;
		inputs_released = 0;
	}

	u32 CurrentSelection = (u32)Context.CurrentCombatOption;
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


	Context.CurrentCombatOption = (CombatMenu::Option)std::max<u32>(std::min<u32>(CurrentSelection, 2), 0);

	if (IsKeyReleased(InputKeys::Action) && Context.CurrentCombatOption == CombatMenu::Option::Attack) 
	{
		bAttacked = true;
		//pitch += 10;
		//lights[0].radius -= 0.1f;
	}
	for(u32 i = 0; i < EnemiesBundle.MaxIndex; i++)
	{
		Enemy& CurrentEnemy = GameEnemies[i];
		if (CurrentEnemy.Engaged && IsIndexValid(EnemiesBundle, i))
		{
			if (CurrentEnemy.CurrentAttackTimer < 0.0f)
			{
				CurrentEnemy.CurrentAttackTimer = CurrentEnemy.AttackTimer;
				//Attack player
			}
			CurrentEnemy.CurrentAttackTimer -= DeltaTime;
	
			if (bAttacked)
			{
				CurrentEnemy.HP -= 25;
				if (CurrentEnemy.HP <= 0)
				{
					RemoveEnemy(i);
					CurrentGameState = GameState::Navigation;
				}
			}
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

	for(u32 i = 0; i < EnemiesBundle.MaxIndex; i++)
	{
		vector2& CurrentLocation = EnemiesLocations[i];
		vector2 DistanceVector;
		DistanceVector.x = CurrentLocation.x - Context.Position.x;
		DistanceVector.y = CurrentLocation.y - Context.Position.y;

		float DP = vector_dotproduct(DistanceVector, DistanceVector);
		if (DP < (Context.Radius*Context.Radius) && !GameEnemies[i].Engaged && IsIndexValid(EnemiesBundle, i))
		{
			GameEnemies[i].Engaged = true;
			//EngagedEnemies[EngagedEnemiesCount] = GameEnemies[i];
			++EngagedEnemiesCount;
			CurrentGameState = GameState::Combat;
			Context.WaitForButtonRelease = true;
		}
	}
}

void TickGame(float DeltaTime)
{
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
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
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
		AddEnemy(21.5f, 1.5f);
		AddEnemy(15.5f, 1.5f);
		AddEnemy(16.0f, 1.8f);
		AddEnemy(16.2f, 1.2f);
		AddEnemy(3.5f,  2.5f);
		AddEnemy(9.5f, 15.5f);
		AddEnemy(10.0f, 15.1f);
		AddEnemy(10.5f, 15.8f);

		uint8_t* pd_screen = pd->graphics->getFrame();
		screen_fb = pd_screen;

		Context.Position.x = 5; 
		Context.Position.y = 5;
		vector2 v;
		v.x = -1.0f;
		v.y = 0.1f;
		vector_normalize(v);
		Context.Direction = v;
		Context.Plane.x = 0.0f; 
		Context.Plane.y = 0.66f;
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

	pd->graphics->setFont(font);
	//pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, 0, 0);
	pd->graphics->markUpdatedRows(0, 240 - 1);
	pd->graphics->clear(kColorWhite);

	uint8_t* pd_screen = pd->graphics->getFrame();


	TickGame(CurrentDeltaMilliseconds);
	render();

	GameTexture& SwordTexture = texture[Context.SwordSprite.texture];
	pd->graphics->drawBitmap(SwordTexture.img, 250, 0, kBitmapUnflipped);
	pd->system->drawFPS(0, 0);

	renderUI();

	return 1;
}

