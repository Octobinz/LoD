#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <vector>
#include <tchar.h>
#include <algorithm>

extern "C"
{
#include "pd_api.h"
}
#include "geometry.h"
#include "gameobject.h"
#include "raycaster.h"
#include "locator.h"
#include "inputs.h"
#include "growarray.h"
#include "enemies.h"
#include "party.h"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const char* fontpathSmall = "Fonts/namco-1x";

LCDFont* font = NULL;
LCDFont* fontSmall = NULL;
unsigned int StartupMilliseconds = 0;
float CurrentDeltaMilliseconds = 0.0f;
unsigned int LastMilliseconds = 0;
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue>

namespace EventSystem
{
	enum Type
	{
		PopupMessage,
		CombatAnim,
		VFX,
		SFX
	};

	struct Event
	{
		Type EventType = PopupMessage;
		bool blocking = false;
		float TimeLeft = 0.0f;
		void* Data = nullptr;
	};
}

struct PopupMessage
{
	char PopupMessage[512];
};

struct VFX
{
	int texture;
	int x;
	int y;
};

std::vector<EventSystem::Event> EventQueue;

template<class T>
T& GetEvent(int index)
{
	return *static_cast<T*>(EventQueue[index].Data);
}

template<class T>
void AddEvent(EventSystem::Type EventType, T* InEvent, float duration, bool blocking)
{
	EventSystem::Event E;
	E.TimeLeft = duration;
	E.EventType = EventType;
	E.Data = InEvent;
	E.blocking = blocking;
	EventQueue.push_back(E);
}

enum CombatTurn
{
	Player,
	Enemies
};


CombatTurn CurrentTurn = Player;
u32 CurrentEnemyTurn = 0;

void RenderFrame() 
{
	if (CurrentGameState != GameState::State::Combat)
	{
		renderFloor();
		renderWalls();
	}

	if (CurrentGameState == GameState::State::Combat)
	{
		RenderCombatBG();
	}
	renderSprites();
}

void QueuePopupMessage(const char* message, float Duration, bool blocking = true)
{
	PopupMessage* Message = new PopupMessage();
	strcpy_s(Message->PopupMessage, 512, message);
	AddEvent<PopupMessage>(EventSystem::Type::PopupMessage, Message, Duration, blocking);
}

void QueueVFX(int InTexture, int x, int y, float Duration, bool blocking = false)
{
	VFX* Effect = new VFX();
	Effect->x = x;
	Effect->y = y;
	Effect->texture = InTexture;
	AddEvent(EventSystem::VFX, Effect, Duration, blocking);
}

void RenderSpellMainMenu()
{
	switch(Context.CurrentSpellOption)
	{
		case CombatMenu::SpellOption::Fire:
		{
			pd->graphics->fillRect(77, 183, 110, 20,  kColorBlack);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("FIRE", strlen("FIRE"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
			pd->graphics->drawText("HEAL", strlen("HEAL"), kASCIIEncoding, 80, 215);
		}
		break;
		case CombatMenu::SpellOption::Heal:
		{
			pd->graphics->fillRect(77, 213, 110, 20,  kColorBlack);
			pd->graphics->drawText("FIRE", strlen("FIRE"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("HEAL", strlen("HEAL"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeCopy );
		}
		break;/*
		case CombatMenu::AttackOption::Thrust:
		{
			pd->graphics->fillRect(197, 183, 110, 20,  kColorBlack);
			pd->graphics->drawText("STAB", strlen("STAB"), kASCIIEncoding, 80, 185);
			pd->graphics->drawText("SWING", strlen("SWING"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("THRUST", strlen("THRUST"), kASCIIEncoding, 200, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
		}
		break;*/
	}
}

void RenderAttackMainMenu()
{
	switch(Context.CurrentAttackOption)
	{
		case CombatMenu::AttackOption::Stab:
		{
			pd->graphics->fillRect(77, 183, 110, 20,  kColorBlack);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("STAB", strlen("STAB"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
			pd->graphics->drawText("SWING", strlen("SWING"), kASCIIEncoding, 80, 215);
			pd->graphics->drawText("THRUST", strlen("THRUST"), kASCIIEncoding, 200, 185);
		}
		break;
		case CombatMenu::AttackOption::Swing:
		{
			pd->graphics->fillRect(77, 213, 110, 20,  kColorBlack);
			pd->graphics->drawText("STAB", strlen("STAB"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("SWING", strlen("SWING"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeCopy );
			pd->graphics->drawText("THRUST", strlen("THRUST"), kASCIIEncoding, 200, 185);
		}
		break;
		case CombatMenu::AttackOption::Thrust:
		{
			pd->graphics->fillRect(197, 183, 110, 20,  kColorBlack);
			pd->graphics->drawText("STAB", strlen("STAB"), kASCIIEncoding, 80, 185);
			pd->graphics->drawText("SWING", strlen("SWING"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("THRUST", strlen("THRUST"), kASCIIEncoding, 200, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
		}
		break;
	}
}

void RenderCombatMainMenu()
{
	switch(Context.CurrentCombatOption)
	{
		case CombatMenu::Option::Attack:
		{
			pd->graphics->fillRect(77, 183, 110, 20,  kColorBlack);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
			pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
			pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
		}
		break;
		case CombatMenu::Option::Spell:
		{
			pd->graphics->fillRect(77, 213, 110, 20,  kColorBlack);
			pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeCopy );
			pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
		}
		break;
		case CombatMenu::Option::Object:
		{
			pd->graphics->fillRect(197, 183, 110, 20,  kColorBlack);
			pd->graphics->drawText("ATTACK", strlen("ATTACK"), kASCIIEncoding, 80, 185);
			pd->graphics->drawText("SPELL", strlen("SPELL"), kASCIIEncoding, 80, 215);
			pd->graphics->setDrawMode( kDrawModeInverted );
			pd->graphics->drawText("ITEMS", strlen("ITEMS"), kASCIIEncoding, 200, 185);
			pd->graphics->setDrawMode( kDrawModeCopy );
		}
		break;
	}
}

void RenderCombatUI()
{
	switch(Context.CurrentCombatMenu)
	{
		case SelectedMenu::Menu::Combat:
			RenderCombatMainMenu();
			break;
		case SelectedMenu::Menu::Attack:
			RenderAttackMainMenu();
			break;
		case SelectedMenu::Menu::Spell:
			RenderSpellMainMenu();
			break;
	}
}

void RenderParty()
{
	float yOffset = 0.0f;
	int CurrentPartyMember = 3;
	for (int i = 0; i < /*CurrentPartyCount*/4; i++)
	{
		GameTexture& UI64Square = texture.Get(Context._64SquareSprite.texture);
		float UIScale = 0.8f;
		if (i == CurrentPartyMember)
			UIScale = 1.0f;
		pd->graphics->drawScaledBitmap(UI64Square.img, 0, yOffset, UIScale,UIScale);

		float MugshotScale = 0.35f;
		if (i == CurrentPartyMember)
			MugshotScale = 0.45f;
		GameTexture& Mugshot = texture.Get(GameParty[i].Mugshot);
		pd->graphics->drawScaledBitmap(Mugshot.img, 3, 4+yOffset, MugshotScale, MugshotScale);
		
		if (i == CurrentPartyMember)
		{
			pd->graphics->setFont(fontSmall);
			pd->graphics->setDrawMode(kDrawModeInverted);
			pd->graphics->drawText(GameParty[i].Name, strlen(GameParty[i].Name), kASCIIEncoding, 8, yOffset + 5);
			pd->graphics->setDrawMode(kDrawModeCopy);
			//pd->graphics->setFont(font);
		}
		yOffset += 64 * UIScale;

	}
}

void RenderVFX(VFX& InVFX)
{
	GameTexture& Effect = texture.Get(InVFX.texture);
	pd->graphics->drawBitmap(Effect.img, InVFX.x, InVFX.y, kBitmapUnflipped);
}

void RenderPopup(PopupMessage* InPopupMessage)
{
/*	char Message[512];
	strcpy_s(Message, strlen(InPopupMessage->PopupMessage), InPopupMessage.PopupMessage);
*/
	GameTexture& UI256Rectangle = texture.Get(Context._256RectangleSprite.texture);
	pd->graphics->drawBitmap(UI256Rectangle.img, 70, 0, kBitmapUnflipped);
	pd->graphics->drawText(InPopupMessage->PopupMessage, strlen(InPopupMessage->PopupMessage), kASCIIEncoding, 80, 10);
}

void renderUI(float DeltaTime)
{
	bool blocked = false;
	if (false == EventQueue.empty())
	{
		for (int i = 0; i < EventQueue.size(); i++)
		{
			EventSystem::Event& M = EventQueue[i];
			switch (M.EventType)
			{
				case EventSystem::Type::PopupMessage:
				{
					RenderPopup(static_cast<PopupMessage*>(M.Data));
				}
				break;
				case EventSystem::Type::VFX:
				{
					RenderVFX(*static_cast<VFX*>(M.Data));
				}
				break;
		
			}
			if (M.blocking)
				continue;
		}
	}

	if (CurrentGameState == GameState::Combat)
	{	
		GameTexture& UI256Rectangle = texture.Get(Context._256RectangleSprite.texture);
		pd->graphics->drawBitmap(UI256Rectangle.img, 70, 175, kBitmapUnflipped);
		RenderCombatUI();
	}

	RenderParty();
}



//returns -1 for player, index for enemy
int GetInitiative()
{
	for(u32 i = 0; i < EngagedEnemiesCount; i++)
	{
		Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[i]];
		if (CurrentEnemy.Engaged && IsIndexValid(EnemiesBundle, EngagedEnemies[i]))
		{
			if(CurrentEnemy.Initiative > Context.Initiative)
			{
				return EngagedEnemies[i];
			}
		} 
	}
	return -1;
}

static LCDBitmap *loadImageAtPath(const char *path)
{
	const char *outErr = nullptr;
	LCDBitmap *img = pd->graphics->loadBitmap(path, &outErr);
	if ( outErr != nullptr ) 
	{
		pd->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
	}
	return img;
}

void RemoveEnemy(u32 index)
{
	Enemy& CurrentEnemy = GameEnemies[index];
	RemoveSprite(GameObjects[CurrentEnemy.IdleObject].ObjectSprite);
	ReleaseIndex(GameObjectsBundle, CurrentEnemy.IdleObject);
	ReleaseIndex(EnemiesLocatorsBundle, CurrentEnemy.Locator);
	ReleaseIndex(EnemiesBundle, index);
}

void AddEnemy(const char* EnemyName, float Initiative, float x, float y)
{
	u32 LocatorIndex = GetNextIndex(EnemiesLocatorsBundle);
	u32 IdleObjectIndex = GetNextIndex(GameObjectsBundle);
	u32 EnemyIndex = GetNextIndex(EnemiesBundle);

	Enemy& CurrentEnemy = GameEnemies[EnemyIndex];
	strcpy_s(CurrentEnemy.EnemyName, 128, EnemyName);
	CurrentEnemy.IdleObject = IdleObjectIndex;
	CurrentEnemy.Locator = LocatorIndex;
	GameObjects[IdleObjectIndex].ObjectSprite = AddSprite("textures/skeleton.png", x, y);
	EnemiesLocations[LocatorIndex].x = x;
	EnemiesLocations[LocatorIndex].y = y;
}

void InitPlayer()
{
	memset(EngagedEnemies, -1, MaxEnemies*sizeof(i32));
	Context.Radius = 1.1f;
	CurrentGameState = GameState::Navigation;
	Context.SwordSprite.texture = LoadTexture("textures/player_sword_1.png");
	Context.ShieldSprite.texture = LoadTexture("textures/player_shield_1.png");
	Context._64SquareSprite.texture = LoadTexture("textures/ui/64Square.png");
	Context.SlashSprite.texture = LoadTexture("textures/ui/slash_effect.png");
	Context._256RectangleSprite.texture = LoadTexture("textures/ui/256Rectangle.png");

	//Create party
	u32 PartyIndex = GetNextIndex(PartyBundle);
	GameParty[PartyIndex].Mugshot = LoadTexture("textures/ui/warrior_mugshot.png");
	strcpy_s(GameParty[PartyIndex].Name, 128, "Player");

	PartyIndex = GetNextIndex(PartyBundle);
	GameParty[PartyIndex].Mugshot = LoadTexture("textures/ui/monk_mugshot.png");
	strcpy_s(GameParty[PartyIndex].Name, 128, "Monk");

	PartyIndex = GetNextIndex(PartyBundle);
	GameParty[PartyIndex].Mugshot = LoadTexture("textures/ui/witch_mugshot.png");
	strcpy_s(GameParty[PartyIndex].Name, 128, "Witch");

	PartyIndex = GetNextIndex(PartyBundle);
	GameParty[PartyIndex].Mugshot = LoadTexture("textures/ui/otter_mugshot.png");
	strcpy_s(GameParty[PartyIndex].Name, 128, "Otter");

}



bool ProcessMenuInputs(float DeltaTime, u32& InOutOption, int ElementsCount)
{
	u32 CurrentSelection = (u32)InOutOption;
	if (IsKeyReleased(InputKeys::Right))
	{
		CurrentSelection += 2;
	}
	if (IsKeyReleased(InputKeys::Left))
	{
		CurrentSelection -= 2;
	}
	if (IsKeyReleased(InputKeys::Up))
	{
		CurrentSelection--;
	}
	if (IsKeyReleased(InputKeys::Down))
	{
		CurrentSelection++;
	}


	InOutOption = std::max<u32>(std::min<u32>(CurrentSelection, ElementsCount-1), 0);

	if (IsKeyReleased(InputKeys::Action)) 
	{
		return true;
	}
	if (IsKeyReleased(InputKeys::Action2)) 
	{
		Context.CurrentCombatMenu = SelectedMenu::Menu::Combat;
		//Context.CurrentCombatMenu = SelectedMenu::Menu::Attack;
	}
	return false;
}

/*
	//Make enemies that should attack attack
	//Check for player attack and damage	
*/
void UpdateCombat(float DeltaTime)
{
	bool bAttacked = false;

	if(Context.WaitForButtonRelease == true)
	{
		if(inputs_down != 0)
			return;
		Context.WaitForButtonRelease = false;
		inputs_released = 0;
	}

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
			u32 Option = (u32)Context.CurrentAttackOption;
			bAttacked = ProcessMenuInputs(DeltaTime, Option, 3);
			Context.CurrentAttackOption = (CombatMenu::AttackOption)Option;
		} break;
		case SelectedMenu::Menu::Spell:
		{
			u32 Option = (u32)Context.CurrentSpellOption;
			bAttacked = ProcessMenuInputs(DeltaTime, Option, 2);
			Context.CurrentSpellOption = (CombatMenu::SpellOption)Option;
		} break;
	}
	
	for(u32 i = 0; i < EngagedEnemiesCount; i++)
	{
		Enemy& CurrentEnemy = GameEnemies[EngagedEnemies[i]];
		if (CurrentEnemy.Engaged && IsIndexValid(EnemiesBundle, EngagedEnemies[i]))
		{
			if (CurrentEnemy.CurrentAttackTimer < 0.0f)
			{
				CurrentEnemy.CurrentAttackTimer = CurrentEnemy.AttackTimer;
				//Attack player
			}
			CurrentEnemy.CurrentAttackTimer -= DeltaTime;

			if (CurrentTurn == CombatTurn::Player)
			{
				if (bAttacked)
				{
					CurrentEnemy.HP -= 25;
					QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
					QueuePopupMessage("25 Damages done!", 1.5f);
					GameTexture& Slash = texture.Get(Context.SlashSprite.texture);

					CurrentTurn = CombatTurn::Enemies;
					if (CurrentEnemy.HP <= 0)
					{
						RemoveEnemy(EngagedEnemies[i]);
						EngagedEnemies[i] = -1;
						memcpy(&EngagedEnemies[i], &EngagedEnemies[i + 1], (MaxEnemies - i)-1);
						EngagedEnemiesCount--;
						if (EngagedEnemiesCount == 0)
							CurrentGameState = GameState::Navigation;
					}
				}
			}
			else
			{
				char Msg[128];
				sprintf(Msg, "%s uses it's sword!", GameEnemies[EngagedEnemies[i]].EnemyName);
				QueueVFX(Context.SlashSprite.texture, 80, -50, 2.0f, false);
				QueuePopupMessage(Msg, 2.0f);
				QueuePopupMessage("15 Damages received!", 1.5f);

				CurrentTurn = CombatTurn::Player;
			}
		}
	}
}

void UpdateNavigation(float DeltaTime)
{
	if (IsKeyReleased(InputKeys::Action)) 
	{
		//pitch += 10;
		//lights[0].radius -= 0.1f;
	}
	const float rotspeed = 3.0f * 0.016f,
	movespeed = 3.0f * 0.016f;

	if (IsKeyDown(InputKeys::Left)) 
	{
		rotate(+rotspeed);
	}

	if (IsKeyDown(InputKeys::Right)) 
	{
		rotate(-rotspeed);
	}


	if (IsKeyDown(InputKeys::Up)) 
	{

		float futurex = Context.Position.x + Context.Direction.x * movespeed;
		float futurey = Context.Position.y + Context.Direction.y * movespeed;
		if(MAPDATA[int(futurex) + int(Context.Position.y) * MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		//if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		if (MAPDATA[int(Context.Position.x) + int(futurey) * MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}

	if (IsKeyDown(InputKeys::Down)) 
	{

		float futurex = Context.Position.x - Context.Direction.x * movespeed;
		float futurey = Context.Position.y - Context.Direction.y * movespeed;

		if(MAPDATA[int(futurex) + int(Context.Position.y)* MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		if (MAPDATA[int(Context.Position.x) + int(futurey)* MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}
	EngagedEnemiesCount = 0;
	for(u32 i = 0; i < EnemiesBundle.MaxIndex; i++)
	{
		vector2& CurrentLocation = EnemiesLocations[i];
		vector2 DistanceVector;
		DistanceVector.x = CurrentLocation.x - Context.Position.x;
		DistanceVector.y = CurrentLocation.y - Context.Position.y;

		float DP = vector_dotproduct(DistanceVector, DistanceVector);
		if (DP < (Context.Radius*Context.Radius) && !GameEnemies[i].Engaged && IsIndexValid(EnemiesBundle, i))
		{
			GameEnemies[i].Engaged = true;
			EngagedEnemies[EngagedEnemiesCount] = i;
			++EngagedEnemiesCount;
			CurrentGameState = GameState::Combat;
			Context.WaitForButtonRelease = true;
		}
	}

	if(CurrentGameState == GameState::Combat)
	{
		int Initiative = GetInitiative();
		if(Initiative == -1)
		{
			QueuePopupMessage("You have the initiative!", 2.0f);
			CurrentTurn = CombatTurn::Player;
		}
		else
		{
			CurrentTurn = CombatTurn::Enemies;
			char Msg[128];
			sprintf(Msg, "%s has the initiative!", GameEnemies[Initiative].EnemyName);
			QueuePopupMessage(Msg, 2.0f);
			
			/*const auto processor_count = std::thread::hardware_concurrency();
			sprintf(Msg, "%d core count!", processor_count);
			QueuePopupMessage(Msg, 2.0f);*/
		}
	}
}

void TickGame(float DeltaTime)
{
	if (!EventQueue.empty())
	{
		for(int i = 0; i < EventQueue.size(); i++)
		{
			EventSystem::Event& M = EventQueue[i];
			M.TimeLeft -= DeltaTime;
			if(M.TimeLeft <= 0.0f)
			{
				delete M.Data;
				EventQueue.pop_back();
				return;
			}
			else
			{
				if(M.blocking)
				{
					return;
				}
			}
		}
	}

	UpdateInputs();

	switch(CurrentGameState)
	{
		case GameState::Combat:
			UpdateCombat(DeltaTime);
			break;
		case GameState::Navigation:
			UpdateNavigation(DeltaTime);
			break;
	}
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
	int eventHandler(PlaydateAPI* InPD, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		pd = InPD;
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		fontSmall = pd->graphics->loadFont(fontpathSmall, &err);
		
		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);
		pd->display->setRefreshRate(50);
		pd->display->setScale(1);		

		LoadTexture( "textures/eagle.png");
		LoadTexture( "textures/blank_rectangle.png");
		LoadTexture( "textures/purplestone.png");
		LoadTexture( "textures/greystone.png");
		LoadTexture( "textures/bluestone.png");
		LoadTexture( "textures/mossy.png");
		LoadTexture( "textures/wood.png");
		LoadTexture( "textures/colorstone.png");
		LoadTexture( "textures/skeleton.png");
		LoadTexture( "textures/pillar.png");
		LoadTexture( "textures/greenlight.png");
		LoadTexture( "textures/woodenfloor.png");
		LoadTexture( "textures/greystone.png");
		LoadTexture( "textures/skeleton.png");

		//memset(EngagedEnemies, -1, sizeof(Enemy)*MaxEnemies);
		InitInputs();
		InitPlayer();
		AddEnemy("Skeleton", 1.0f, 21.5f, 1.5f);
		AddEnemy("Skeleton", 1.0f, 15.5f, 1.5f);
		AddEnemy("Skeleton", 1.0f, 16.0f, 1.8f);
		AddEnemy("Skeleton", 1.0f, 16.2f, 1.2f);
		AddEnemy("Skeleton", 1.0f, 3.5f,  2.5f);
		AddEnemy("Skeleton", 1.0f, 9.5f, 15.5f);
		AddEnemy("Skeleton", 1.0f, 10.0f, 15.1f);
		AddEnemy("Skeleton", 1.0f, 10.5f, 15.8f);

		AddLight(1.0f, 1.0f, 20.0f);
		AddLight(8.0f, 8.0f, 2.5f);

		uint8_t* pd_screen = pd->graphics->getFrame();
		screen_fb = pd_screen;

		Context.Position.x = 5; 
		Context.Position.y = 5;

		vector2 v = {-1.0f, 0.1f};
		vector_normalize(v);
		Context.Direction = v;
		Context.Plane = { 0.0f, 0.66f };
		StartupMilliseconds = pd->system->getCurrentTimeMilliseconds();
		CurrentDeltaMilliseconds = 0;
		LastMilliseconds = StartupMilliseconds;

	}
	
	return 0;
}

static int offset = 0;
static int update(void* userdata)
{
	pd = (PlaydateAPI *)userdata;
	unsigned int MS;
	MS = pd->system->getCurrentTimeMilliseconds();
	CurrentDeltaMilliseconds = (MS - LastMilliseconds) / 1000.0f;
	LastMilliseconds = MS;

	pd->graphics->setFont(fontSmall);
	//pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, 0, 0);
	pd->graphics->markUpdatedRows(0, 240 - 1);
	pd->graphics->clear(kColorWhite);

	uint8_t* pd_screen = pd->graphics->getFrame();


	TickGame(CurrentDeltaMilliseconds);
	RenderFrame();

	GameTexture& SwordTexture = texture.Get(Context.SwordSprite.texture);
	pd->graphics->drawBitmap(SwordTexture.img, 250, 0, kBitmapUnflipped);
	pd->system->drawFPS(0, 0);

	renderUI(CurrentDeltaMilliseconds);

	return 1;
}

