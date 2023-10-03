#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <stdbool.h>
#include <queue>

#include "types.h"

#ifndef _WINDLL
#include <pdcpp/pdnewlib.h>
#endif

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
		case GameMode::ScrollingIntro:
			TickScrollingIntro(DeltaTime);
			break;
	}
}

#ifndef _WINDLL
extern "C" {
#endif

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* InPD, PDSystemEvent event, uint32_t arg)
{

	if (event == kEventInit)
	{
#ifndef _WINDLL
		eventHandler_pdnewlib(InPD, event, arg);
#endif
		pd = InPD;
		pd->system->setUpdateCallback(update, pd);
		uint8_t* pd_screen = pd->graphics->getFrame();
		screen_fb = pd_screen;
		
		//Load fonts		
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		fontSmall = pd->graphics->loadFont(fontpathSmall, &err);

		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->display->setRefreshRate(50);
		pd->display->setScale(1);		
		InPD->system->logToConsole("Nom de dieu blake");

		InitRaycaster();

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
		AddLight(1.0f, 1.0f, 2.0f);
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

#ifndef _WINDLL
}
#endif

static int offset = 0;
static int update(void* userdata)
{
# if 1
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
#endif
	return 1;
}

#if 0
#include <memory>
#include <string>

constexpr int TEXT_WIDTH = 86;
constexpr int TEXT_HEIGHT = 16;

/**
 * This Class contains the entire logic of the "game"
 */
class HelloWorld
{
public:
	explicit HelloWorld(PlaydateAPI* api)
		: pd(api)
		, fontpath("/System/Fonts/Asheville-Sans-14-Bold.pft")
		, x((400 - TEXT_WIDTH) / 2)
		, y((240 - TEXT_HEIGHT) / 2)
		, dx(1), dy(2)
	{
		const char* err;
		font = pd->graphics->loadFont(fontpath.c_str(), &err);

		if (font == nullptr)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath.c_str(), err);
	}

	void update()
	{
		pd->graphics->clear(kColorWhite);
		pd->graphics->setFont(font);
		pd->graphics->drawText("Hello C++!", strlen("Hello World!"), kASCIIEncoding, x, y);

		x += dx;
		y += dy;

		if (x < 0 || x > LCD_COLUMNS - TEXT_WIDTH)
		{
			dx = -dx;
		}

		if (y < 0 || y > LCD_ROWS - TEXT_HEIGHT)
		{
			dy = -dy;
		}

		pd->system->drawFPS(0, 0);
	}

private:
	PlaydateAPI* pd;
	std::string fontpath;
	LCDFont* font;
	int x, y, dx, dy;
};

/**
 * You can use STL containers! Be careful with some stdlib objects which rely
 * on an OS though, those will cause your app to crash on launch.
 */
std::unique_ptr<HelloWorld> helloWorld;


/**
 * The Playdate API requires a C-style, or static function to be called as the
 * main update function. Here we use such a function to delegate execution to
 * our class.
 */
static int gameTick(void* userdata)
{
	helloWorld->update();
	return 1;
};


/**
 * the `eventHandler` function is the entry point for all Playdate applications
 * and Lua extensions. It requires C-style linkage (no name mangling) so we
 * must wrap the entire thing in this `extern "C" {` block
 */
#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * This is the main event handler. Using the `Init` and `Terminate` events, we
	 * allocate and free the `HelloWorld` handler accordingly.
	 *
	 * You only need this `_WINDLL` block if you want to run this on a simulator on
	 * a windows machine, but for the sake of broad compatibility, we'll leave it
	 * here.
	 */
#ifdef _WINDLL
	__declspec(dllexport)
#endif
		int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
	{
		/*
		 * This is required, otherwise linker errors abound
		 */
#ifndef _WINDLL
		eventHandler_pdnewlib(pd, event, arg);
#endif
		// Initialization just creates our "game" object
		if (event == kEventInit)
		{
			pd->display->setRefreshRate(20);
			helloWorld = std::make_unique<HelloWorld>(pd);

			// and sets the tick function to be called on update to turn off the
			// typical 'Lua'-ness.
			pd->system->setUpdateCallback(gameTick, pd);
		}

		// Destroy the global state to prevent memory leaks
		if (event == kEventTerminate)
		{
			pd->system->logToConsole("Tearing down...");
			helloWorld = nullptr;
		}
		return 0;
	}
#ifdef __cplusplus
}
#endif
#endif