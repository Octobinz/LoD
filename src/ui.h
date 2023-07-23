#pragma once

#include "types.h"
#include <vector>

extern "C"
{
#include "pd_api.h"
}

namespace EventSystem
{
	enum Type
	{
		PopupMessage,
		CombatAnim,
		DialogueMessage,
		VFX,
		SFX
	};

	struct Event
	{
		Type EventType = PopupMessage;
		bool blocking = false;
		bool processed = false;
		float TimeLeft = 0.0f;
		void* Data = nullptr;
	};
}

extern LCDFont* font;
extern LCDFont* fontSmall;

extern const char* fontpath;
extern const char* fontpathSmall;

struct PopupMessage
{
	char Message[512];
};

struct DialogueMessage : public PopupMessage
{
	int PartyMember1;
	int PartyMember2;
};

struct VFX
{
	int texture;
	int x;
	int y;
};

extern std::vector<EventSystem::Event> EventQueue;

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
	E.processed = false;
	EventQueue.push_back(E);
}

void RenderSpellMainMenu();
void RenderAttackMainMenu();
void RenderCombatMainMenu();
void RenderCombatUI();
void RenderParty();
void RenderVFX(VFX& InVFX);
void RenderPopup(PopupMessage* InPopupMessage);
void RenderDialogue(DialogueMessage* InDialogueMessage);
void renderUI(float DeltaTime);

void QueuePopupMessage(const char* message, float Duration, bool blocking = true);
void QueueDialogueMessage(const char* message, float Duration, bool blocking = true);
void QueueVFX(int InTexture, int x, int y, float Duration, bool blocking = false);
