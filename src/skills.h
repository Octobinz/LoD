#pragma once

#include "gameobject.h"
#include "enemies.h"
#include "growarray.h"

typedef void(*PlayerActionFunction)(int, int);
typedef void(*EnemyActionFunction)(int, int);

struct GameTurn
{
	bool IsPartyMember = false;
	float Initiative = 0.0f;
	int index = -1;
};

extern u32 CurrentCombatIndex;
extern GrowArray<GameTurn> GameTurns;

struct GameSkill
{
	char Name[32];
	void PE(int PlayerIndex, int InEnemy);//Player to enemy
	void EP(int InEnemy, int PlayerIndex);//Enemy to player
	PlayerActionFunction PlayerAction = nullptr;
	EnemyActionFunction EnemyAction = nullptr;
};

void skill_stab_PE(int PlayerIndex, int InEnemy);
void skill_stab_EP(int InEnemy, int PlayerIndex);

void skill_thrust_PE(int PlayerIndex, int InEnemy);
void skill_thrust_EP(int InEnemy, int PlayerIndex);

void skill_swing_PE(int PlayerIndex, int InEnemy);
void skill_swing_EP(int InEnemy, int PlayerIndex);

extern GameSkill Stab;
extern GameSkill Swing;
extern GameSkill Thrust;
extern GameSkill WarriorLevels[][3];

extern GrowArray<GameSkill> CurrentGameSkills;