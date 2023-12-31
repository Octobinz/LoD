#include "ui.h"
#include "gameobject.h"
#include "party.h"
#include "raycaster.h"
#include "inputs.h"
#include "game.h"
#include "combat.h"

const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const char* fontpathSmall = "Fonts/namco-1x";

LCDFont* font = nullptr;
LCDFont* fontSmall = nullptr;

GrowArray<EventSystem::Event> EventQueue;


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
	int XOffset = 0;
	int YOffset = 0;
	int Index = 0;

	GameTurn& G = GameTurns.Get(CurrentCombatIndex);
	int PartyMemberIndex = G.index;
	if (G.IsPartyMember)
	{
		for (u32 i = 0; i < Party[PartyMemberIndex].Skills.size(); i++)
		{
			GameSkill& Skill = Party[PartyMemberIndex].Skills[i];
			if (YOffset >= 60)
			{
				YOffset = 0;
				XOffset += 120;
			}
			if (Index == Context.CurrentAttackOption.index)
			{
				pd->graphics->fillRect(77 + XOffset, 183 + YOffset, 110, 20,  kColorBlack);
				pd->graphics->setDrawMode(kDrawModeInverted);
			}

			pd->graphics->drawText(Skill.Name, strlen(Skill.Name), kASCIIEncoding, 80 + XOffset, 185 + YOffset);
			pd->graphics->setDrawMode(kDrawModeCopy);

			YOffset += 30;
			++Index;
		}
	}
#if 0
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
#endif
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
	for (u32 i = 0; i < Party.size(); i++)
	{
		GameTexture& UI64Square = *UsedTextures.Get(Context._64SquareSprite.texture);
		float UIScale = 0.8f;
		if (i == CurrentPartyContext.CurrentPartyMember)
			UIScale = 1.0f;
		pd->graphics->drawScaledBitmap(UI64Square.img, 0, (int)yOffset, UIScale,UIScale);

		float MugshotScale = 0.35f;
		if (i == CurrentPartyContext.CurrentPartyMember)
			MugshotScale = 0.45f;
		GameTexture& Mugshot = *UsedTextures.Get(Party[i].Mugshot);
		pd->graphics->drawScaledBitmap(Mugshot.img, 3, 4+(int)yOffset, MugshotScale, MugshotScale);
		
		if (i == CurrentPartyContext.CurrentPartyMember)
		{
			pd->graphics->setFont(fontSmall);
			pd->graphics->setDrawMode(kDrawModeInverted);
			pd->graphics->drawText(Party[i].Name, strlen(Party[i].Name), kASCIIEncoding, 8, (int)yOffset + 5);
			pd->graphics->setDrawMode(kDrawModeCopy);
			//pd->graphics->setFont(font);
		}
		yOffset += 64 * UIScale;

	}
}

void RenderVFX(VFX& InVFX)
{
	GameTexture& Effect = *UsedTextures.Get(InVFX.texture);
	pd->graphics->drawBitmap(Effect.img, InVFX.x, InVFX.y, kBitmapUnflipped);
}

void RenderPopup(PopupMessage* InPopupMessage)
{
	/*	char Message[512];
	strcpy_s(Message, strlen(InPopupMessage->PopupMessage), InPopupMessage.PopupMessage);
	*/
	GameTexture& UI256Rectangle = *UsedTextures.Get(Context._256RectangleSprite.texture);
	pd->graphics->drawBitmap(UI256Rectangle.img, 70, 175, kBitmapUnflipped);
	pd->graphics->drawText(InPopupMessage->Message, strlen(InPopupMessage->Message), kASCIIEncoding, 80, 185);
}

void RenderDialogue(DialogueMessage* InDialogueMessage)
{
	/*	char Message[512];
	strcpy_s(Message, strlen(InPopupMessage->PopupMessage), InPopupMessage.PopupMessage);
	*/
	GameTexture& UI256Rectangle = *UsedTextures.Get(Context._256RectangleSprite.texture);
	pd->graphics->drawBitmap(UI256Rectangle.img, 70, 175, kBitmapUnflipped);
	pd->graphics->drawText(InDialogueMessage->Message, strlen(InDialogueMessage->Message), kASCIIEncoding, 80, 10);
}

void RenderGameUI(float DeltaTime)
{
	bool blocked = false;
	bool bShouldRenderCombatUI = true;
	if (false == EventQueue.empty())
	{
		for (int i = 0; i < EventQueue.size(); i++)
		{
			EventSystem::Event& M = EventQueue[i];
			M.processed = true;
			switch (M.EventType)
			{
				case EventSystem::Type::PopupMessage:
				{
					bShouldRenderCombatUI = false;
					RenderPopup(static_cast<PopupMessage*>(M.Data));
				}
				break;
				case EventSystem::Type::VFX:
				{
					bShouldRenderCombatUI = false;
					RenderVFX(*static_cast<VFX*>(M.Data));
				}
				break;
				case EventSystem::Type::DialogueMessage:
				{
					bShouldRenderCombatUI = false;
					RenderDialogue(static_cast<DialogueMessage*>(M.Data));
				}
				break;
		
			}
			if (M.blocking)
				break;
		}
	}

	if (CurrentGameState == GameState::Combat &&
		bShouldRenderCombatUI)
	{	
		GameTexture& UI256Rectangle = *UsedTextures.Get(Context._256RectangleSprite.texture);
		pd->graphics->drawBitmap(UI256Rectangle.img, 70, 175, kBitmapUnflipped);
		RenderCombatUI();
	}

	RenderParty();
}

void RenderMainMenuUI(float DeltaTime)
{
	GameTexture& MainTitleSprite = *UsedTextures.Get(Context.MainTitleSprite.texture);
	pd->graphics->drawBitmap(MainTitleSprite.img, 0, 0, kBitmapUnflipped);
}

static float scrollY = 50;
void RenderScrollingIntroUI(float DeltaTime)
{
	pd->graphics->clear(kColorWhite);
	scrollY -= DeltaTime * 15.0f;
	pd->graphics->drawText("In a world where", strlen("In a world where"), kASCIIEncoding, 20, scrollY);
	pd->graphics->drawText("mistery is misterious", strlen("mistery is misterious"), kASCIIEncoding, 20, scrollY + 15);

}


void renderUI(float DeltaTime)
{
	switch(CurrentGameMode)
	{
		case GameMode::InGame:
		{
			RenderGameUI(DeltaTime);
		}
		break;
		case GameMode::MainMenu:
			RenderMainMenuUI(DeltaTime);
			break;
		case GameMode::ScrollingIntro:
			RenderScrollingIntroUI(DeltaTime);
			break;
	}
}

void QueuePopupMessage(const char* message, float Duration, bool blocking)
{
	PopupMessage* Message = new PopupMessage();
	strcpy(Message->Message, message);
	AddEvent<PopupMessage>(EventSystem::Type::PopupMessage, Message, Duration, blocking);
}

void QueueDialogueMessage(const char* message, float Duration, bool blocking )
{
	DialogueMessage* Message = new DialogueMessage();
	strcpy(Message->Message, message);
	AddEvent<PopupMessage>(EventSystem::Type::PopupMessage, Message, Duration, blocking);
}


void QueueVFX(int InTexture, int x, int y, float Duration, bool blocking )
{
	VFX* Effect = new VFX();
	Effect->x = x;
	Effect->y = y;
	Effect->texture = InTexture;
	AddEvent(EventSystem::VFX, Effect, Duration, blocking);
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