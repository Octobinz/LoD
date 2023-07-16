#pragma once

#include "gameobject.h"


enum InputKeys
{
	Left,
	Right,
	Up,
	Down,
	Action,
	Max
};

extern bool inputs_released[InputKeys::Max];
extern bool inputs_down[InputKeys::Max];

void UpdateInputs();
void InitInputs();