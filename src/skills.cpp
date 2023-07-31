#include "gameobject.h"
#include "enemies.h"
#include "skills.h"
#include "ui.h"
#include "party.h"
#include "raycaster.h"
#include "combat.h"


void CalcAndApplyDamageToEnemy(int Damage, int EnemyIndex)
{
	Damage -= GameEnemies[EnemyIndex].Armor;

	QueueEnemyDamage(EnemyIndex, Damage);

	char Msg[128];
	sprintf(Msg, "%d damages done!", Damage);
	QueuePopupMessage(Msg, 1.5f);
}

void CalcAndApplyDamageToPartyMember(int Damage, int PlayerIndex)
{
	Damage -= Party[PlayerIndex].Armor;
	QueuePlayerDamage(&Party[PlayerIndex], 25);

	char Msg[128];
	sprintf(Msg, "%s received %d damages !", Party[PlayerIndex].Name, Damage);
	QueuePopupMessage(Msg, 1.5f);
}

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

void skill_stab_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(25, InEnemy);
}

void skill_stab_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

void skill_swing_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(150, InEnemy);
}

void skill_swing_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

void skill_thrust_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(25, InEnemy);
}

void skill_thrust_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

void skill_fire_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(25, InEnemy);
}

void skill_fire_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

void skill_ice_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(25, InEnemy);
}

void skill_ice_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

void skill_focus_PE(int PlayerIndex, int InEnemy)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToEnemy(25, InEnemy);
}

void skill_focus_EP(int InEnemy, int PlayerIndex)
{
	QueueVFX(Context.SlashSprite.texture, 80, -50, 1.0f, false);
	CalcAndApplyDamageToPartyMember(15, PlayerIndex);
}

GameSkill Stab 		= { "Stab", 	skill_stab_PE, 		skill_stab_EP };
GameSkill Swing 	= { "Swing", 	skill_swing_PE, 	skill_swing_EP };
GameSkill Thrust 	= { "Thrust", 	skill_thrust_PE, 	skill_thrust_EP };
GameSkill Rage 		= { "Rage", 	skill_thrust_PE, 	skill_thrust_EP };
GameSkill Provoke 	= { "Provoke", 	skill_thrust_PE, 	skill_thrust_EP };
GameSkill Fire 		= { "Fire", 	skill_fire_PE, 		skill_fire_EP };
GameSkill Ice 		= { "Ice", 		skill_ice_PE, 		skill_ice_EP };
GameSkill Focus 	= { "Focus", 	skill_focus_PE, 	skill_focus_EP };

GameSkill WarriorLevels[][3] = { { Stab, Swing, Thrust },
								{ Rage },
								{ Provoke },
};

GameSkill WitchLevels[][3] = { { Fire, Ice, Focus },
};

GameSkill MeleeEnemyLevels[][3] = { { Stab, Swing, Thrust },
									{ Rage },
									{ Provoke }, 
};
GrowArray<GameSkill> CurrentGameSkills;


