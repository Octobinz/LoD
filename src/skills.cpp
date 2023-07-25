#include "gameobject.h"
#include "enemies.h"
#include "skills.h"
#include "ui.h"
#include "party.h"
#include "raycaster.h"
#include "combat.h"

void GameSkill::PE(int PlayerIndex, int InEnemy)
{
	char Msg[128];
	sprintf(Msg, "%s uses %s!", Party[PlayerIndex].Name, this->Name);
	QueuePopupMessage(Msg, 1.5f);

	PlayerAction(PlayerIndex, InEnemy);
}

void GameSkill::EP(int InEnemy, int PlayerIndex)
{
	char Msg[128];
	sprintf(Msg, "%s uses %s!", GameEnemies[InEnemy].Name, this->Name);
	QueuePopupMessage(Msg, 1.5f);
	EnemyAction(InEnemy, PlayerIndex);
}

void skill_stab_PE(int PlayerIndex, int Enemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	QueueEnemyDamage(Enemy, 25);
	QueuePopupMessage("25 Damages done!", 1.5f);
}

void skill_stab_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	QueuePlayerDamage(&Party[PlayerIndex], 25);
	QueuePopupMessage("25 Damages done!", 1.5f);
}

void skill_swing_PE(int PlayerIndex, int InEnemy)
{
}

void skill_swing_EP(int InEnemy, int PlayerIndex)
{
}

void skill_thrust_PE(int PlayerIndex, int InEnemy)
{
}

void skill_thrust_EP(int InEnemy, int PlayerIndex)
{
}

GameSkill Stab 		= { "Stab", 	skill_stab_PE, 		skill_stab_EP };
GameSkill Swing 	= { "Swing", 	skill_swing_PE, 	skill_swing_EP };
GameSkill Thrust 	= { "Thrust", 	skill_thrust_PE, 	skill_thrust_EP };
GameSkill Rage 		= { "Rage", 	skill_thrust_PE, 	skill_thrust_EP };
GameSkill Provoke 	= { "Provoke", 	skill_thrust_PE, 	skill_thrust_EP };

GameSkill WarriorLevels[][3] = { { Stab, Swing, Thrust },
								{ Rage },
								{ Provoke },
};


GrowArray<GameSkill> CurrentGameSkills;


