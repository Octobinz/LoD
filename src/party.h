#pragma once

#include "gameobject.h"
#include "enemies.h"
#include "skills.h"

namespace CharacterClass
{
	enum Type
	{
		Warrior,
		Witch,
		Priest,
		Max
	};
}


//extern GameSkill AvailableSkills[];

struct PartyMember
{
	char Name[256];
	int IdleObject;
	int AttackObject;
	int Mugshot;
	int Locator;
	GrowArray<GameSkill> Skills;
	//Attack timer, hp, endurance etc...
	float AttackTimer = 1.0f;// In seconds
	float CurrentAttackTimer = 1.0f;// In seconds
	float Initiative = 1.0f;
	
	int HP = 100;
	int Level = 1;
	int Armor = 10;
	bool Engaged = false;

	CharacterClass::Type Class = CharacterClass::Warrior;
};



struct PartyContext
{
	int CurrentPartyMember = -1;
};

extern PartyContext CurrentPartyContext;
extern GrowArray<PartyMember> Party;
extern GrowArray<i32> EngagedParty;
extern GrowArray<vector2> PartyLocations;
extern u8 EngagedPartyCount;

void AddPartyMember(const char* MemberName, CharacterClass::Type InClass, const char* Mugshot, float Initiative, int InLevel);
void SetPartyMemberClass(CharacterClass::Type InClass);
void SetPartyMemberLevel(int Index, int Level);