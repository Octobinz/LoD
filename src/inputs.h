#pragma once

#include "gameobject.h"


enum InputKeys
{
	Left,
	Right,
	Up,
	Down,
	Action,
	Action2,
	Max
};

extern u32 inputs_released;
extern u32 inputs_down;

void UpdateInputs();
void InitInputs();
bool IsKeyDown(InputKeys InInputKey);
bool IsKeyReleased(InputKeys InInputKey);