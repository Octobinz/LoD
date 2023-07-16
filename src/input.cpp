#include "inputs.h"

extern "C"
{
	#include "pd_api.h"
}

bool inputs_released[InputKeys::Max];
bool inputs_down[InputKeys::Max];

void InitInputs()
{
	for(int i = 0; i < InputKeys::Max; i++)
	{
		inputs_down[i] = false;
		inputs_released[i] = false;
	}
}

void UpdateInputs()
{
	PDButtons current;
	pd->system->getButtonState(&current, nullptr, nullptr);
	
	if (current & kButtonLeft)
	{
		inputs_released[InputKeys::Left] = false;
		inputs_down[InputKeys::Left] = true;
	}
	else
	{
		inputs_released[InputKeys::Left] = inputs_down[InputKeys::Left];
		inputs_down[InputKeys::Left] = false;
	}

	//Right
	if (current & kButtonRight)
	{
		inputs_released[InputKeys::Right] = false;
		inputs_down[InputKeys::Right] = true;
	}
	else
	{
		inputs_released[InputKeys::Right] = inputs_down[InputKeys::Right];
		inputs_down[InputKeys::Right] = false;
	}

	//Up
	if (current & kButtonUp)
	{
		inputs_released[InputKeys::Up] = false;
		inputs_down[InputKeys::Up] = true;
	}
	else
	{
		inputs_released[InputKeys::Up] = inputs_down[InputKeys::Up];
		inputs_down[InputKeys::Up] = false;
	}

	//Down
	if (current & kButtonDown)
	{
		inputs_released[InputKeys::Down] = false;
		inputs_down[InputKeys::Down] = true;
	}
	else
	{
		inputs_released[InputKeys::Down] = inputs_down[InputKeys::Down];
		inputs_down[InputKeys::Down] = false;
	}


	if (current & kButtonB)
	{
		inputs_released[InputKeys::Action] = false;
		inputs_down[InputKeys::Action] = true;
	}
	else
	{
		inputs_released[InputKeys::Action] = inputs_down[InputKeys::Action];
		inputs_down[InputKeys::Action] = false;
	}
}