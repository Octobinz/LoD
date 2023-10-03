#include "inputs.h"

u32 inputs_released;
u32 inputs_down;

void InitInputs()
{
	inputs_down = 0;
	inputs_released = 0;
}

bool IsKeyDown(InputKeys InInputKey)
{
	return inputs_down & (1 << InInputKey);
}

bool IsKeyReleased(InputKeys InInputKey)
{
	return inputs_released & (1 << InInputKey);
}

FORCEINLINE void SetValue(u32& buffer, InputKeys index, bool value)
{
	if(value)
	{
		buffer |= (1 << index);
	}
	else
	{
		buffer &= ~(1 << index);
	}
}

void UpdateInputs()
{
	PDButtons current;
	pd->system->getButtonState(&current, nullptr, nullptr);
	
	if (current & kButtonLeft)
	{
		SetValue(inputs_released, InputKeys::Left, false);
		SetValue(inputs_down, InputKeys::Left, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Left, inputs_down & (1<<InputKeys::Left));
		SetValue(inputs_down, InputKeys::Left, false);
	}

	//Right
	if (current & kButtonRight)
	{
		SetValue(inputs_released, InputKeys::Right, false);
		SetValue(inputs_down, InputKeys::Right, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Right, inputs_down & (1<<InputKeys::Right));
		SetValue(inputs_down, InputKeys::Right, false);
	}

	//Up
	if (current & kButtonUp)
	{
		SetValue(inputs_released, InputKeys::Up, false);
		SetValue(inputs_down, InputKeys::Up, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Up, inputs_down & (1<<InputKeys::Up));
		SetValue(inputs_down, InputKeys::Up, false);
	}

	//Down
	if (current & kButtonDown)
	{
		SetValue(inputs_released, InputKeys::Down, false);
		SetValue(inputs_down, InputKeys::Down, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Down, inputs_down & (1<<InputKeys::Down));
		SetValue(inputs_down, InputKeys::Down, false);
	}


	if (current & kButtonB)
	{
		SetValue(inputs_released, InputKeys::Action, false);
		SetValue(inputs_down, InputKeys::Action, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Action, inputs_down & (1<<InputKeys::Action));
		SetValue(inputs_down, InputKeys::Action, false);
	}

	if (current & kButtonA)
	{
		SetValue(inputs_released, InputKeys::Action2, false);
		SetValue(inputs_down, InputKeys::Action2, true);
	}
	else
	{
		SetValue(inputs_released, InputKeys::Action2, inputs_down & (1<<InputKeys::Action2));
		SetValue(inputs_down, InputKeys::Action2, false);
	}
}