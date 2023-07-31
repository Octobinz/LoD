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
#include "game.h"
#include "mainmenu.h"

static int update(void* userdata);

unsigned int StartupMilliseconds = 0;
float CurrentDeltaMilliseconds = 0.0f;
unsigned int LastMilliseconds = 0;

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue>

void RenderFrame(float DeltaTime) 
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
	renderSprites(DeltaTime);
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


void InitPlayer()
{
	GameEnemies.Preallocate(MaxEnemies);
	EnemiesLocations.Preallocate(MaxEnemies);

	CurrentGameState = GameState::Navigation;
	Context.SwordSprite.texture = LoadTexture("textures/player_sword_1.png");
	Context.ShieldSprite.texture = LoadTexture("textures/player_shield_1.png");
	Context._64SquareSprite.texture = LoadTexture("textures/ui/64Square.png");
	Context.SlashSprite.texture = LoadTexture("textures/ui/slash_effect.png");
	Context._256RectangleSprite.texture = LoadTexture("textures/ui/256Rectangle.png");
	Context.MainTitleSprite.texture = LoadTexture("textures/ui/MainTitle.png");
	//Create party
	AddPartyMember("Player", CharacterClass::Type::Warrior, "textures/ui/warrior_mugshot.png", 1.0f, 1);
	AddPartyMember("Priest", CharacterClass::Type::Priest,"textures/ui/monk_mugshot.png", 2.0f, 1);
	AddPartyMember("Witch", CharacterClass::Type::Witch,"textures/ui/witch_mugshot.png", 0.5f, 1);
	AddPartyMember("Otter", CharacterClass::Type::Warrior,"textures/ui/otter_mugshot.png", 7.0f, 1);
	
	Party[3].Level++;
	//QueueDialogueMessage("I think I sense a trap", 5.0f, true);
}

void Tick(float DeltaTime)
{
	switch(CurrentGameMode)
	{
		case GameMode::InGame:
		{
			TickGame(DeltaTime);
			RenderFrame(DeltaTime);
		}
		break;
		case GameMode::MainMenu:
			TickMainMenu(DeltaTime);
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

		LoadTexture( "textures/stonefloor.png");
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
		//AddEnemy("Skeleton", 1.0f, 3.5f,  2.5f);
		AddEnemy("Skeleton", 1.0f, 9.5f, 15.5f);
		AddEnemy("Skeleton", 1.0f, 10.0f, 15.1f);
		AddEnemy("Skeleton", 1.0f, 10.5f, 15.8f);

		//AddSprite("textures/Barrel.png", 2.0f, 0.9f);
		AddLight(1.0f, 1.0f, 20.0f);
		AddLight(8.0f, 8.0f, 2.5f);

		Context.Position.x = 1.5; 
		Context.Position.y = 1.5;

		vector2 v = {-1.0f, 0.1f};
		vector_normalize(v);
		Context.Direction = v;
		Context.Plane = { 0.0f, 0.66f };
		StartupMilliseconds = pd->system->getCurrentTimeMilliseconds();
		CurrentDeltaMilliseconds = 0;
		LastMilliseconds = StartupMilliseconds;
		
		sfxplayer = pd->sound->fileplayer->newPlayer();
		musicplayer = pd->sound->fileplayer->newPlayer();


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

	Tick(CurrentDeltaMilliseconds);

/*
	GameTexture& SwordTexture = texture.Get(Context.SwordSprite.texture);
	pd->graphics->drawBitmap(SwordTexture.img, 250, 0, kBitmapUnflipped);
	pd->system->drawFPS(0, 0);
*/
	renderUI(CurrentDeltaMilliseconds);
	pd->system->drawFPS(0, 0);

	return 1;
}

