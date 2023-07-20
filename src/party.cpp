#pragma once

#include "party.h"

PartyMember GameParty[MaxParty];
EntityBundle<PartyMember> PartyBundle;
vector2 PartyLocations[MaxParty];
u8 EngagedPartyCount = 0;
u8 CurrentPartyCount = 0;
