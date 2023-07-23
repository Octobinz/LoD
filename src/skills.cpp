#include "gameobject.h"
#include "enemies.h"
#include "skills.h"
#include "ui.h"
#include "party.h"

void skill_stab_PE(int PlayerIndex, Enemy& InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
	QueuePopupMessage("25 Damages done!", 1.5f);
	InEnemy.HP -= 25;
}

void skill_stab_EP(Enemy& InEnemy, int PlayerIndex)
{
	Party[PlayerIndex].HP -= 25;
}

GameSkill Stab = { "Stab", skill_stab_PE, skill_stab_EP };
GameSkill Swing = { "Swing", skill_stab_PE, skill_stab_EP };
GameSkill Thrust = { "Thrust", skill_stab_PE, skill_stab_EP };