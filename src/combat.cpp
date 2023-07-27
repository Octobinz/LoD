#include "combat.h"
#include "gameobject.h"
#include "inputs.h"
#include "skills.h"
#include "ui.h"
#include "party.h"
#include "raycaster.h"

CombatTurn CurrentTurn = Player;


void QueueKillEnemy(int InEnemy)
{
	KillEnemyMessage* msg = new KillEnemyMessage();
	msg->Target = InEnemy;
	AddEvent(EventSystem::KillEnemy, msg, 0.1f, true);
}

void QueuePlayerDamage(PartyMember* InTarget, int DamageAmount, bool blocking)
{
	ApplyDamageToPartyMember* Effect = new ApplyDamageToPartyMember();
	Effect->TargetPartyMember = InTarget;
	Effect->DamageAmount = DamageAmount;
	AddEvent(EventSystem::PlayerHurt, Effect, 0.1f, blocking);
}

void QueueEnemyDamage(int InTarget, int DamageAmount, bool blocking)
{
	ApplyDamageToEnemy* Effect = new ApplyDamageToEnemy();
	Effect->TargetEnemy = InTarget;
	Effect->DamageAmount = DamageAmount;
	AddEvent(EventSystem::EnemyHurt, Effect, 0.1f, blocking);
}

//true if attacked
bool UpdateCombatMenu(float DeltaTime)
{
	bool bAttacked = false;
	switch(Context.CurrentCombatMenu)
	{
		case SelectedMenu::Menu::Combat:
		{
			u32 Option = (u32)Context.CurrentCombatOption;
			if(ProcessMenuInputs(DeltaTime,Option,3))
			{
				switch(Option)
				{
					case 0:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Attack;
						break;
					case 1:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Spell;
						break;
					case 2:
						Context.CurrentCombatMenu = SelectedMenu::Menu::Item;
						break;
				}
			}
			Context.CurrentCombatOption = (CombatMenu::Option)Option;
		} break;
		case SelectedMenu::Menu::Attack:
		{
			CurrentGameSkills.Reset();
			GameTurn CurrentGameTurn = GameTurns.Get(CurrentCombatIndex);
			
			if (CurrentGameTurn.IsPartyMember)
			{
				PartyMember& Member = Party[CurrentGameTurn.index];
				for (int i = 0; i < Member.Skills.size(); i++)
				{
					CurrentGameSkills.push_back(Member.Skills[i]);
				}

				u32 Option = Context.CurrentAttackOption.index;
				bAttacked = ProcessMenuInputs(DeltaTime, Option, CurrentGameSkills.size());
				Context.CurrentAttackOption.index = Option;
			}
		} break;
		case SelectedMenu::Menu::Spell:
		{
			u32 Option = (u32)Context.CurrentSpellOption;
			bAttacked = ProcessMenuInputs(DeltaTime, Option, 2);
			Context.CurrentSpellOption = (CombatMenu::SpellOption)Option;
		} break;
	}

	return bAttacked;
}

u32 CurrentCombatIndex = 0;
GrowArray<GameTurn> GameTurns;

void GenerateGameTurns()
{
	GameTurns.Reset();

	for(u32 i = 0; i < EngagedEnemies.size(); i++)
	{
		Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[i]];
		GameTurn NewGameTurn;
		if (CurrentEnemy.Engaged && 
			EngagedEnemies[i] < GameEnemies.size())
		{
			NewGameTurn.IsPartyMember = false;
			NewGameTurn.index = EngagedEnemies[i];
			NewGameTurn.Initiative = CurrentEnemy.Initiative;
			GameTurns.AddElement(NewGameTurn);
		} 
	}

	for(u32 i = 0; i < Party.size(); i++)
	{
		PartyMember& CurrentPartyMember = Party[i];
		GameTurn NewGameTurn;

		NewGameTurn.IsPartyMember = true;
		NewGameTurn.index = i;
		NewGameTurn.Initiative = CurrentPartyMember.Initiative;
		GameTurns.AddElement(NewGameTurn);
	}
	
	//Sort game turns by Initiative
	for(u32 i = 0; i < GameTurns.size(); i++)
	{
		for(u32 j = 0; j < GameTurns.size(); j++)
		{
			if(GameTurns[i].Initiative > GameTurns[j].Initiative)
			{
				GameTurn NewGameTurn;
				std::swap(GameTurns[i], GameTurns[j]);
			}
		}
	}
}

/*
	//Make enemies that should attack attack
	//Check for player attack and damage	
	*/
void UpdateCombat(float DeltaTime)
{

	if(Context.WaitForButtonRelease == true)
	{
		if(inputs_down != 0)
			return;
		Context.WaitForButtonRelease = false;
		inputs_released = 0;
	}

	GameTurn CurrentGameTurn = GameTurns.Get(CurrentCombatIndex);
	CurrentTurn = CurrentGameTurn.IsPartyMember ? CombatTurn::Player : CombatTurn::Enemies;

	bool bAttacked = UpdateCombatMenu(DeltaTime);
	
	if (CurrentTurn == CombatTurn::Player)
	{
		CurrentPartyContext.CurrentPartyMember = CurrentGameTurn.index;

		if (bAttacked)
		{
			int EnemyIndex = 0;
			Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[EnemyIndex]];
			if (CurrentEnemy.Engaged && 
				EngagedEnemies[EnemyIndex] < GameEnemies.size())
			{
				GameSkill Skill = CurrentGameSkills[Context.CurrentAttackOption.index];
				Skill.PE(CurrentGameTurn.index, EngagedEnemies[EnemyIndex]);
			}			
			CurrentCombatIndex++;
			if(CurrentCombatIndex >= GameTurns.size())
			{
				CurrentCombatIndex = 0;
			}
		}
	}
	else
	{
		Enemy& CurrentEnemy = GameEnemies[CurrentGameTurn.index];
		int skillIndex = randomInRange(0, CurrentEnemy.Skills.size()-1);
		int playerIndex = randomInRange(0, Party.size()-1);
		GameSkill& skill = CurrentEnemy.Skills[skillIndex];
		skill.EP(CurrentGameTurn.index, skillIndex);

		CurrentCombatIndex++;
		if(CurrentCombatIndex > GameTurns.size())
		{
			CurrentCombatIndex = 0;
		}
	}

}
