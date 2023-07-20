#pragma once

#include "enemies.h"

i32 EngagedEnemies[MaxEnemies];
Enemy GameEnemies[MaxEnemies];
EntityBundle<Enemy> EnemiesBundle;
vector2 EnemiesLocations[MaxEnemies];
u8 EngagedEnemiesCount = 0;
u8 CurrentEnemiesCount = 0;
