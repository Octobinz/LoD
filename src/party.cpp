#pragma once

#include "party.h"
#include "raycaster.h"

PartyMember Party[MaxParty];
EntityBundle<PartyMember> PartyBundle;
vector2 PartyLocations[MaxParty];
u8 EngagedPartyCount = 0;
u8 CurrentPartyCount = 0;
PartyContext CurrentPartyContext;

void AddPartyMember(const char* MemberName, const char* Mugshot, float Initiative)
{
	CurrentPartyCount++;
	u32 PartyIndex = GetNextIndex(PartyBundle);
	Party[PartyIndex].Mugshot = LoadTexture(Mugshot);
	Party[PartyIndex].Initiative = Initiative;
	strcpy_s(Party[PartyIndex].Name, 64, MemberName);
}