#pragma once

#include "gameobject.h"
#include "enemies.h"
#include "skills.h"

static const u32 MaxParty = 4;

namespace CharacterClass
{
	enum Type
	{
		Warrior,
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

	bool Engaged = false;
	//Attack timer, hp, endurance etc...
	float AttackTimer = 1.0f;// In seconds
	float CurrentAttackTimer = 1.0f;// In seconds
	float Initiative = 1.0f;
	int HP = 100;
	CharacterClass::Type Type = CharacterClass::Warrior;
};



struct PartyContext
{
	int CurrentPartyMember = -1;
};

extern PartyContext CurrentPartyContext;
extern PartyMember Party[MaxParty];
extern i32 EngagedParty[MaxParty];
extern vector2 PartyLocations[MaxParty];
extern EntityBundle<PartyMember> PartyBundle;
extern u8 EngagedPartyCount;
extern u8 CurrentPartyCount;

void AddPartyMember(const char* MemberName, const char* Mugshot, float Initiative);