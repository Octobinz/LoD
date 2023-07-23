#pragma once

#include "gameobject.h"
#include "enemies.h"
#include "growarray.h"

typedef void(*PlayerActionFunction)(int, Enemy&);
typedef void(*EnemyActionFunction)(Enemy&, int);

struct GameTurn
{
	bool IsPartyMember = false;
	float Initiative = 0.0f;
	int index = -1;
};

extern int CurrentCombatIndex;
extern GrowArray<GameTurn> GameTurns;

struct GameSkill
{
	char Name[32];
	void PE(int PlayerIndex, Enemy& InEnemy);//Player to enemy
	void EP(Enemy& InEnemy, int PlayerIndex);//Enemy to player
	PlayerActionFunction PlayerAction = nullptr;
	EnemyActionFunction EnemyAction = nullptr;
};

void skill_stab_PE(int PlayerIndex, Enemy& InEnemy);
void skill_stab_EP(Enemy& InEnemy, int PlayerIndex);

void skill_thrust_PE(int PlayerIndex, Enemy& InEnemy);
void skill_thrust_EP(Enemy& InEnemy, int PlayerIndex);

void skill_swing_PE(int PlayerIndex, Enemy& InEnemy);
void skill_swing_EP(Enemy& InEnemy, int PlayerIndex);

extern GameSkill Stab;
extern GameSkill Swing;
extern GameSkill Thrust;
extern GameSkill WarriorLevels[][3];