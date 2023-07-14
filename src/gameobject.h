#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "geometry.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
#define SCREEN_WIDTH 416
#define SCREEN_HEIGHT 240

struct Sprite
{
	float x;
	float y;
	int texture;
};

struct Light
{
	vector3 location;
	float radius;
};

struct GameTexture
{
	u8* texture;
	u8* mask;
	u32 width;
	u32 height;
	u32 rowbyte;
	LCDBitmap *img;
};

struct PlayerContext
{
	vector2 Position; 
	vector2 Direction;
	vector2 Plane;
	Sprite SwordSprite;
	Sprite ShieldSprite;
};

struct GameObject
{
	matrix_4x4 transform;
};