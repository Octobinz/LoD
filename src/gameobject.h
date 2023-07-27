#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "geometry.h"
#include "types.h"
#include "entity.h"
#include <time.h>

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
	enum SpellOption
	{
		Fire,
		Heal
	};
}

namespace SelectedMenu
{
	enum Menu: u32
	{
		Combat,
		Attack,
		Spell,
		Item
	};
}

struct GameObject;

static const u32 MaxGameObjects = 256;

extern PlaydateAPI* pd;
extern FilePlayer* musicplayer;
extern FilePlayer* sfxplayer;

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
	float originalx;
	float originaly;
	float scale = 1.0f;
	int texture = -1;
	float shakeRemainingTime; // Variable to store the starting time of shaking
	int shakeAmount;      // Duration of the shake in seconds
};


// Function to generate a random number within a given range
int randomInRange(int min, int max);
// Function to shake the sprite for a given duration
void shakeSprite(struct Sprite *sprite, float shakeAmount, float deltaTime); 
void InitSpriteShake(struct Sprite *sprite, float shakeDuration, int InShakeAmount);

struct AttackOption
{
	//struct GameSkill* Skill;
	int index = 0;
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
	Sprite SlashSprite;
	Sprite MainTitleSprite;

	CombatMenu::Option CurrentCombatOption = CombatMenu::Option::Attack;
	AttackOption CurrentAttackOption;
	CombatMenu::SpellOption CurrentSpellOption = CombatMenu::SpellOption::Fire;

	SelectedMenu::Menu CurrentCombatMenu = SelectedMenu::Menu::Combat;
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

extern GameObject GameObjects[MaxGameObjects];
extern vector2 ObjectsLocations[MaxGameObjects];
extern u8 CurrentGameObjectCount;
extern u8 CurrentEnemyLocatorCount;
extern EntityBundle<GameObject> GameObjectsBundle;

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