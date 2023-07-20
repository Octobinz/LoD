#pragma once

#include "gameobject.h"

static const u32 MaxEnemies = 256;

namespace EnemyType
{
	enum Type
	{
		Skeleton,
		Max
	};
}

struct Enemy
{
	char EnemyName[128];
	int IdleObject;
	int AttackObject;
	int Locator;

	bool Engaged = false;
	//Attack timer, hp, endurance etc...
	float AttackTimer = 1.0f;// In seconds
	float CurrentAttackTimer = 1.0f;// In seconds
	float Initiative = 1.0f;
	int HP = 100;
	EnemyType::Type Type = EnemyType::Skeleton;
};

extern Enemy GameEnemies[MaxEnemies];
extern i32 EngagedEnemies[MaxEnemies];
extern vector2 EnemiesLocations[MaxGameObjects];
extern EntityBundle<Enemy> EnemiesBundle;
extern u8 EngagedEnemiesCount;
extern u8 CurrentEnemiesCount;
