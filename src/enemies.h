#pragma once

#include "gameobject.h"
#include "growarray.h"
#include "skills.h"

static const u32 MaxEnemies = 32;

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
	char Name[128];
	int IdleObject;
	int AttackObject;
	int Locator;
	bool Engaged = false;
	//Attack timer, hp, endurance etc...
	float AttackTimer = 1.0f;// In seconds
	float CurrentAttackTimer = 1.0f;// In seconds
	float Initiative = 1.0f;
	int HP = 100;
	int PlayerTarger = 0;
	GrowArray<GameSkill> Skills;
	EnemyType::Type Type = EnemyType::Skeleton;
};

extern GrowArray<Enemy> GameEnemies;
extern GrowArray<u32> EngagedEnemies;
extern GrowArray<vector2> EnemiesLocations;
extern EntityBundle<Enemy> EnemiesBundle;

void AddEnemiesClosestTo(vector2& InLocation, float Radius);
void RemoveEnemy(u32 index);
void AddEnemy(const char* EnemyName, float Initiative, float x, float y);