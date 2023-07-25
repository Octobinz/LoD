#pragma once

enum CombatTurn
{
	Player,
	Enemies
};

struct ApplyDamageToPartyMember
{
	struct PartyMember* TargetPartyMember = nullptr;
	int DamageAmount = 0;
};


struct ApplyDamageToEnemy
{
	int TargetEnemy;
	int DamageAmount = 0;
};

struct KillEnemyMessage
{
	int Target;
};

extern CombatTurn CurrentTurn;

void UpdateCombat(float DeltaTime);
void GenerateGameTurns();

void QueueKillEnemy(int InEnemy);
void QueuePlayerDamage(PartyMember* InTarget, int DamageAmount, bool blocking = true);
void QueueEnemyDamage(int InTarget, int DamageAmount, bool blocking = true);