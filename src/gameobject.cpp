#include "gameobject.h"

PlayerContext Context;
GameObject GameObjects[MaxGameObjects];
EntityBundle<GameObject> GameObjectsBundle;

vector2 ObjectsLocations[MaxGameObjects];

GameState::State CurrentGameState = GameState::State::Max;
u8 CurrentGameObjectCount = 0;
u8 CurrentLocatorCount = 0;
PlaydateAPI* pd = nullptr;