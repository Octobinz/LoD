#include "gameobject.h"
#include "enemies.h"
#include "skills.h"
#include "ui.h"
#include "party.h"

void GameSkill::PE(int PlayerIndex, Enemy& InEnemy)
{
	char Msg[128];
	sprintf(Msg, "%s uses %s!", Party[PlayerIndex].Name, this->Name);
	QueuePopupMessage(Msg, 1.5f);

	PlayerAction(PlayerIndex, InEnemy);
}

void GameSkill::EP(Enemy& InEnemy, int PlayerIndex)
{
	char Msg[128];
	sprintf(Msg, "%s uses %s!", InEnemy.Name, this->Name);
	QueuePopupMessage(Msg, 1.5f);
	EnemyAction(InEnemy, PlayerIndex);
}

void skill_stab_PE(int PlayerIndex, Enemy& InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
	QueuePopupMessage("25 Damages done!", 1.5f);
	InEnemy.HP -= 25;
}

void skill_stab_EP(Enemy& InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
	QueuePopupMessage("25 Damages done!", 1.5f);
	Party[PlayerIndex].HP -= 25;
}

void skill_swing_PE(int PlayerIndex, Enemy& InEnemy)
{
}

void skill_swing_EP(Enemy& InEnemy, int PlayerIndex)
{
}

void skill_thrust_PE(int PlayerIndex, Enemy& InEnemy)
{
}

void skill_thrust_EP(Enemy& InEnemy, int PlayerIndex)
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


