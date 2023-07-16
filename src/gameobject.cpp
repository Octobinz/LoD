#include "gameobject.h"

PlayerContext Context;
GameObject GameObjects[MaxGameObjects];
EntityBundle<GameObject> GameObjectsBundle;
EntityBundle<Enemy> EnemiesBundle;

Enemy GameEnemies[MaxEnemies];

vector2 ObjectsLocations[MaxGameObjects];
vector2 EnemiesLocations[MaxEnemies];

Enemy EngagedEnemies[MaxEnemies];

GameState::State CurrentGameState = GameState::State::Max;
u8 EngagedEnemiesCount = 0;
u8 CurrentEnemiesCount = 0;
u8 CurrentGameObjectCount = 0;
u8 CurrentLocatorCount = 0;
PlaydateAPI* pd = nullptr;