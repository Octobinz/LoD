#pragma once

#include "party.h"
#include "raycaster.h"
#include "growarray.h"

GrowArray<PartyMember> Party;
GrowArray<vector2> PartyLocations;
u8 EngagedPartyCount = 0;
PartyContext CurrentPartyContext;

void AddPartyMember(const char* MemberName, CharacterClass::Type InClass, const char* Mugshot, float Initiative, int InLevel)
{
	PartyMember PM;
	PM.Mugshot = LoadTexture(Mugshot);
	PM.Initiative = Initiative;
	PM.Class = InClass;
	strcpy_s(PM.Name, 64, MemberName);
	Party.push_back(PM);

	SetPartyMemberLevel(Party.size()-1, InLevel);
}

#define COPY_CLASS_SKILLS(X) \
	for (int i = 0; i < PM.Level; i++) \
	{\
		for (int j = 0; j < sizeof(X[i]) / sizeof(GameSkill); j++) \
		{\
			GameSkill& Skill = X[i][j]; \
			PM.Skills.push_back(Skill); \
		} \
	}

void SetPartyMemberClass(int Index, CharacterClass::Type InClass)
{
	PartyMember& PM = Party[Index];
	PM.Skills.Reset();

	switch(InClass)
	{
		case CharacterClass::Type::Warrior:
		{
			COPY_CLASS_SKILLS(WarriorLevels)
		}
		break;
		case CharacterClass::Type::Witch:
		{
			COPY_CLASS_SKILLS(WitchLevels)
			
		} 
		break;
		case CharacterClass::Type::Priest:
		{
			COPY_CLASS_SKILLS(WitchLevels)
		} 
		break;
	}
}

void SetPartyMemberLevel(int Index, int Level)
{
	PartyMember& PM = Party[Index];
	PM.Level = Level;
	SetPartyMemberClass(Index, PM.Class);
}