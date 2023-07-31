#include "gameobject.h"

PlayerContext Context;
GameObject GameObjects[MaxGameObjects];
EntityBundle<GameObject> GameObjectsBundle;

vector2 ObjectsLocations[MaxGameObjects];

GameState::State CurrentGameState = GameState::State::Max;
u8 CurrentGameObjectCount = 0;
u8 CurrentLocatorCount = 0;
PlaydateAPI* pd = nullptr;
FilePlayer* sfxplayer = nullptr;
FilePlayer* musicplayer = nullptr;

// Function to generate a random number within a given range
int randomInRange(int min, int max) 
{
	return rand() % (max - min + 1) + min;
}

// Function to initialize the sprite shake
void InitSpriteShake(struct Sprite *sprite, float shakeDuration, int InShakeAmount) 
{
	sprite->originalx = sprite->x;
	sprite->originaly = sprite->y;
	sprite->shakeRemainingTime = shakeDuration;
	sprite->shakeAmount = InShakeAmount;
}

// Function to shake the sprite for a given duration
void shakeSprite(struct Sprite *sprite, int shakeAmount, float deltaTime) 
{
	if (sprite->shakeRemainingTime <= 0.0f)
	{
		sprite->x = sprite->originalx;
		sprite->y = sprite->originaly;
		return;
	}

	sprite->shakeRemainingTime -= deltaTime;

	float startX = sprite->x;
	float startY = sprite->y;
	float endX, endY;

	endX = startX + randomInRange(-shakeAmount, shakeAmount) / 500.0f;
	endY = startY + randomInRange(-shakeAmount, shakeAmount) / 500.0f;

	// Set the new coordinates of the sprite
	sprite->x = endX;
	sprite->y = endY;
}