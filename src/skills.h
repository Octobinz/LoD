#pragma once

#include "gameobject.h"
#include "enemies.h"


typedef void(*PlayerActionFunction)(int, Enemy&);
typedef void(*EnemyActionFunction)(Enemy&, int);

struct GameSkill
{
	char Name[32];
	PlayerActionFunction PlayerAction = nullptr;
	EnemyActionFunction EnemyAction = nullptr;
};

void skill_stab(int PlayerIndex, Enemy& InEnemy);

extern GameSkill Stab;
extern GameSkill Swing;
extern GameSkill Thrust;