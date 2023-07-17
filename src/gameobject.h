#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "geometry.h"
#include "types.h"
#include "entity.h"

extern "C"
{
#include "pd_api.h"
}


#define SCREEN_WIDTH 416
#define SCREEN_HEIGHT 240
#define HALF_SCREEN_HEIGHT 120

namespace CombatMenu
{
	enum Option : u32
	{
		Attack,
		Spell,
		Object
	};
}

namespace SelectedMenu
{
	enum Menu: u32
	{
		Attack,
		Spell,
		Object
	};
}

struct GameObject;

static const u32 MaxGameObjects = 256;
static const u32 MaxEnemies = 256;

extern PlaydateAPI* pd;

struct Light
{
	vector3 location;
	float radius;
};

struct GameTexture
{
	u8* texture;
	u8* mask;
	u32 width;
	u32 height;
	u32 rowbyte;
	LCDBitmap *img;
};

struct Sprite
{
	float x;
	float y;
	float scale = 1.0f;
	int texture = -1;
};


struct PlayerContext
{
	vector2 Position; 
	vector2 Direction;
	vector2 Plane;
	Sprite SwordSprite;
	Sprite ShieldSprite;

	Sprite _64SquareSprite;
	Sprite _256RectangleSprite;
	CombatMenu::Option CurrentCombatOption = CombatMenu::Option::Attack;
	float Radius = 1.0f;
	float Initiative = 0.1f;
	bool WaitForButtonRelease = false;

};

extern PlayerContext Context;



/*
	Some object that can be found in the game world	
*/
//Locator index is the same as the GameObject index
struct GameObject
{
	int ObjectSprite;
	float Radius = 1.0f;
};

struct Enemy
{
	char EnemyName[128];
	int Object;
	int Locator;
	bool Engaged = false;
	float AttackTimer = 1.0f;// In seconds
	float CurrentAttackTimer = 1.0f;// In seconds
	float Initiative = 1.0f;
	int HP = 100;

	//Attack timer, hp, endurance etc...
};

extern GameObject GameObjects[MaxGameObjects];
extern Enemy GameEnemies[MaxEnemies];
extern vector2 EnemiesLocations[MaxGameObjects];
extern vector2 ObjectsLocations[MaxGameObjects];
//extern Enemy EngagedEnemies[MaxEnemies];
extern u8 EngagedEnemiesCount;
extern u8 CurrentEnemiesCount;
extern u8 CurrentGameObjectCount;
extern u8 CurrentEnemyLocatorCount;
extern EntityBundle<GameObject> GameObjectsBundle;
extern EntityBundle<Enemy> EnemiesBundle;

namespace GameState
{
	enum State
	{
		MainMenu,
		Navigation,
		Combat,
		Death,
		Max
	};
};

extern GameState::State CurrentGameState;