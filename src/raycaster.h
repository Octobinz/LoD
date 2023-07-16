#pragma once

#include "entity.h"

extern uint8_t *screen_fb;		// frame buffer

#define MAP_SIZE 24
extern u8 MAPDATA[MAP_SIZE * MAP_SIZE];
#define samplepixel(data, x, y, rowbytes) (((data[(y)*rowbytes+((x)>>3)] & (1 << (uint8_t)(7 - ((x) & 7)))) != 0) ? kColorWhite : kColorBlack)
extern std::vector<GameTexture> texture;

static const u32 MaxSprites = 512;
extern Sprite Sprites[MaxSprites];
extern EntityBundle<Sprite> SpritesBundle;

void renderFloor();
void renderSprites();
void renderWalls();
int LoadTexture(const char* filename);
void rotate(float rot);
int AddSprite(const char* filename, float x, float y);
void RemoveSprite(int index);