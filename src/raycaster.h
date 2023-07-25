#pragma once

#include "entity.h"
#include "growarray.h"

extern uint8_t *screen_fb;		// frame buffer

#define MAP_SIZE 24
extern u8 MAPDATA[MAP_SIZE * MAP_SIZE];
#define samplepixel(data, x, y, rowbytes) (((data[(y)*rowbytes+((x)>>3)] & (1 << (uint8_t)(7 - ((x) & 7)))) != 0) ? kColorWhite : kColorBlack)
extern GrowArray<GameTexture> texture;

static const u32 MaxSprites = 512;
static const u32 MaxLights = 32;
extern Sprite Sprites[MaxSprites];
extern Light Lights[MaxLights];
extern EntityBundle<Sprite> SpritesBundle;

void renderFloor();
void renderSprites(float DeltaTime);
void renderWalls();
void RenderCombatBG();
int LoadTexture(const char* filename);
void rotate(float rot);
int AddSprite(const char* filename, float x, float y);
void RemoveSprite(int index);
int AddLight(float x, float y, float radius);
void RemoveLight(int index);