#pragma once

#include "enemies.h"
#include "raycaster.h"
#include "locator.h"

GrowArray<u32> EngagedEnemies;
GrowArray<Enemy> GameEnemies;
EntityBundle<Enemy> EnemiesBundle;
GrowArray<vector2> EnemiesLocations;


void RemoveEnemy(u32 index)
{
	Enemy& CurrentEnemy = GameEnemies[index];
	RemoveSprite(GameObjects[CurrentEnemy.IdleObject].ObjectSprite);
	ReleaseIndex(GameObjectsBundle, CurrentEnemy.IdleObject);
	ReleaseIndex(EnemiesLocatorsBundle, CurrentEnemy.Locator);
	ReleaseIndex(EnemiesBundle, index);
}

void SetEnemyLevel(int InEnemy, int level)
{
	Enemy& CurrentEnemy = GameEnemies[InEnemy];
	CurrentEnemy.Skills.Reset();

	for (int i = 0; i < level; i++)
	{
		for (int j = 0; j < sizeof(MeleeEnemyLevels[i]) / sizeof(GameSkill); j++)
		{
			GameSkill& Skill = MeleeEnemyLevels[i][j];
			CurrentEnemy.Skills.push_back(Skill);
		}
	}
}

void AddEnemy(const char* EnemyName, float Initiative, float x, float y)
{
	u32 LocatorIndex = GetNextIndex(EnemiesLocatorsBundle);
	u32 IdleObjectIndex = GetNextIndex(GameObjectsBundle);
	u32 EnemyIndex = GetNextIndex(EnemiesBundle);

	Enemy& CurrentEnemy = GameEnemies[EnemyIndex];
	CurrentEnemy.Engaged = false;
	strcpy_s(CurrentEnemy.Name, 128, EnemyName);
	CurrentEnemy.IdleObject = IdleObjectIndex;
	CurrentEnemy.Locator = LocatorIndex;
	
	SetEnemyLevel(EnemyIndex, 1);

	GameObjects[IdleObjectIndex].ObjectSprite = AddSprite("textures/skeleton.png", x, y);
	EnemiesLocations[LocatorIndex].x = x;
	EnemiesLocations[LocatorIndex].y = y;
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
			EngagedEnemies.push_back(i);
			CurrentGameState = GameState::Combat;
			Context.WaitForButtonRelease = true;
			AddEnemiesClosestTo(CurrentLocation, 1.5f);
		}
	}
}