#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <vector>
#include <tchar.h>

extern "C"
{
	#include "pd_api.h"
}
#include "geometry.h"
#include "gameobject.h"
#include "raycaster.h"


uint8_t *screen_fb = NULL;		// frame buffer


float ZBuffer[SCREEN_WIDTH];

#define numSprites 19

std::vector<GameTexture> texture;
EntityBundle<Sprite> SpritesBundle;
EntityBundle<Light> LightsBundle;

Sprite Sprites[MaxSprites];
Light Lights[MaxLights];

int AddLight(float x, float y, float radius)
{
	u32 Index = GetNextIndex(LightsBundle);
	Light& CurrentLight = Lights[Index];
	CurrentLight.location.x = x;
	CurrentLight.location.y = y;
	CurrentLight.radius = radius;
	return Index;
}

void RemoveLight(int index)
{
	ReleaseIndex(LightsBundle, index);
}

int AddSprite(const char* filename, float x, float y)
{
	u32 Index = GetNextIndex(SpritesBundle);
	Sprite& CurrentSprite = Sprites[Index];
	CurrentSprite.texture = LoadTexture(filename);
	CurrentSprite.x = x;
	CurrentSprite.y = y;
	return Index;
}

void RemoveSprite(int index)
{
	Sprite& CurrentSprite = Sprites[index];
	CurrentSprite.texture = -1;
	ReleaseIndex(SpritesBundle, index);
}

int LoadTexture(const char* filename)
{
	const char *outErr = NULL;
	LCDBitmap *img = pd->graphics->loadBitmap(filename, &outErr);
		
	int width; 
	int height; 
	int rowbytes; 
	uint8_t* mask; 
	uint8_t* data; 
	pd->graphics->getBitmapData(img, &width, &height, &rowbytes, &mask, &data);
	int index = texture.size();
	texture.push_back(GameTexture());

	texture[index].texture = (u8*)malloc(width * height);
	texture[index].mask = (u8*)malloc(width * height);
	texture[index].width = width;
	texture[index].height = height;
	texture[index].rowbyte = rowbytes;
	texture[index].img = img;
	memcpy(texture[index].texture, data, (width*height) / 8);
	
	if (mask != nullptr)
	{
		memcpy(texture[index].mask, mask, (width*height) / 8);
	}
	return index;
}

//arrays used to sort the sprites
int SpriteOrder[MaxSprites];
float SpriteDistance[MaxSprites];

void SortSprites(int (&SpriteOrder)[MaxSprites])
{
	for (u32 i = 0; i < SpritesBundle.MaxIndex; i++)
	{
		SpriteOrder[i] = i;
		SpriteDistance[i] = ((Context.Position.x - Sprites[i].x) * (Context.Position.x - Sprites[i].x) + (Context.Position.y - Sprites[i].y) * (Context.Position.y - Sprites[i].y));
	}

	for(u32 i = 0; i < SpritesBundle.MaxIndex; i++) 
	{
		for(u32 j = 0; j < SpritesBundle.MaxIndex; j++) 
		{
			float Distance1 = SpriteDistance[SpriteOrder[i]];
			float Distance2 = SpriteDistance[SpriteOrder[j]];

			if(Distance1 > Distance2)
			{
				int t = SpriteOrder[i];
				SpriteOrder[i] = SpriteOrder[j];
				SpriteOrder[j] = t;
			}
		}		
	}
}

float GetLightAttenuation(vector3& InPosition, Light& InLight)
{
	const vector3 v = vector3(InPosition.x - InLight.location.x, InPosition.y - InLight.location.y, InPosition.z - InLight.location.z);
	float Distance = (vector_length(v) + 0.1f);
	float ratio =  std::max(1.0f - (Distance / InLight.radius), 0.0f);
	return ratio;
}

u8 MAPDATA[MAP_SIZE * MAP_SIZE] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
	1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,1,
	1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,
	1,0,0,0,1,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,1,0,1,
	1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,1,1,0,1,
	1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,5,5,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};


int pitch = 0;
float posZ = 0; // vertical camera strafing up/down, for jumping/crouching. 0 means standard height. Expressed in screen pixels a wall at distance 1 shifts

void renderFloor()
{
	//FLOOR CASTING
	for(int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		// whether this section is floor or ceiling
		bool is_floor = y > HALF_SCREEN_HEIGHT + pitch;

		// rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
		float rayDirX0 = Context.Direction.x - Context.Plane.x;
		float rayDirY0 = Context.Direction.y - Context.Plane.y;
		float rayDirX1 = Context.Direction.x + Context.Plane.x;
		float rayDirY1 = Context.Direction.y + Context.Plane.y;

		// Current y position compared to the center of the screen (the horizon)
		int p = is_floor ? (y - HALF_SCREEN_HEIGHT - pitch) : (HALF_SCREEN_HEIGHT - y + pitch);

		// Vertical position of the camera.
		// NOTE: with 0.5, it's exactly in the center between floor and ceiling,
		// matching also how the walls are being raycasted. For different values
		// than 0.5, a separate loop must be done for ceiling and floor since
		// they're no longer symmetrical.
		float camZ = is_floor ? (0.5f * SCREEN_HEIGHT + posZ) : (0.5f * SCREEN_HEIGHT - posZ);

		// Horizontal distance from the camera to the floor for the current row.
		// 0.5 is the z position exactly in the middle between floor and ceiling.
		// NOTE: this is affine texture mapping, which is not perspective correct
		// except for perfectly horizontal and vertical surfaces like the floor.
		// NOTE: this formula is explained as follows: The camera ray goes through
		// the following two points: the camera itself, which is at a certain
		// height (posZ), and a point in front of the camera (through an imagined
		// vertical plane containing the screen pixels) with horizontal distance
		// 1 from the camera, and vertical position p lower than posZ (posZ - p). When going
		// through that point, the line has vertically traveled by p units and
		// horizontally by 1 unit. To hit the floor, it instead needs to travel by
		// posZ units. It will travel the same ratio horizontally. The ratio was
		// 1 / p for going through the camera plane, so to go posZ times farther
		// to reach the floor, we get that the total horizontal distance is posZ / p.
		float rowDistance = camZ / p;

		// calculate the real world step vector we have to add for each x (parallel to camera plane)
		// adding step by step avoids multiplications with a weight in the inner loop
		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;

		// real world coordinates of the leftmost column. This will be updated as we step to the right.
		float floorX = Context.Position.x + rowDistance * rayDirX0;
		float floorY = Context.Position.y + rowDistance * rayDirY0;

		for(int x = 0; x < SCREEN_WIDTH; ++x)
		{
			// the cell coord is simply got from the integer parts of floorX and floorY
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);

			// choose texture and draw the pixel
			int checkerBoardPattern = (int(cellX + cellY)) & 1;
			int floorTexture = 11;
			int ceilingTexture = 6;

			// get the texture coordinate from the fractional part
			int tx = (int)(texture[floorTexture].width * (floorX - cellX)) & (texture[floorTexture].width - 1);
			int ty = (int)(texture[floorTexture].height * (floorY - cellY)) & (texture[floorTexture].height - 1);

			floorX += floorStepX;
			floorY += floorStepY;

			u32 color;

			vector3 PixelWorldPosition;
			PixelWorldPosition.x = floorX;
			PixelWorldPosition.y = floorY;
			PixelWorldPosition.z = 0.5f;

			float LightAttenuation = 0.0f;
			for(u32 i = 0; i < LightsBundle.MaxIndex; i++)
				LightAttenuation += GetLightAttenuation(PixelWorldPosition, Lights[i]);

			if(is_floor) 
			{
				// floor
				color = samplepixel(texture[floorTexture].texture, tx, ty, texture[floorTexture].rowbyte);
				int index = (y * SCREEN_WIDTH) + x;
				int byteIndex = index / 8;
				int bitOffset = index % 8;
				bool Penumbra = false; //(LightAttenuation < 0.2f && (tx % 2 && ty % 2));
				bool bBlack = (color == 0) || (LightAttenuation < 0.01f) || Penumbra;

				if (bBlack)
				{
					screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
				}
				else
				{
					screen_fb[byteIndex] |= (1 << (7 - bitOffset));
				}
			} 
			else 
			{
				//ceiling
				color = samplepixel(texture[1].texture, tx, ty, texture[1].rowbyte);

				//color = texture[ceilingTexture][texWidth * ty + tx];
				int index = (y * SCREEN_WIDTH) + x;
				int byteIndex = index / 8;
				int bitOffset = index % 8;
				int LightModulo = (1.0f - LightAttenuation) < 0.1f ? 1 : (int)((1.0f - LightAttenuation)*5.0f);
				bool Penumbra = false; //(LightAttenuation < 0.9f && (tx % LightModulo && ty % LightModulo));
				bool bBlack = (color == 0) || LightAttenuation < 0.01f || Penumbra;

				if (bBlack)
				{
					screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
				}
				else
				{
					screen_fb[byteIndex] |= (1 << (7 - bitOffset));
				}			
			}
		}
	}
}

void renderWalls()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		//calculate ray position and direction
		//float cameraX = 2 * x / (float)SCREEN_WIDTH - 1; //x-coordinate in camera space
		const float xcam = (2 * (x / (float) (SCREEN_WIDTH))) - 1;
		vector2 Plane = Context.Plane;
		vector2 RayDirection;
		vertex_mul(Plane, xcam);
		vector_add(&RayDirection, &Context.Direction, &Plane);
	
		//which box of the map we're in
		int mapX = int(Context.Position.x);
		int mapY = int(Context.Position.y);
		
		//length of ray from current position to next x or y-side
		float sideDistX;
		float sideDistY;
	
		//length of ray from one x or y-side to next x or y-side
		vector2 deltaDist;
		deltaDist.x = (RayDirection.x == 0.0f) ? 1e30 : std::abs(1.0f / RayDirection.x);
		deltaDist.y = (RayDirection.y == 0.0f) ? 1e30 : std::abs(1.0f / RayDirection.y);
		float perpWallDist;
	
		//what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;
	
		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?
	
		//calculate step and initial sideDist
		if (RayDirection.x < 0)
		{
			stepX = -1;
			sideDistX = (Context.Position.x - mapX) * deltaDist.x;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0f - Context.Position.x) * deltaDist.x;
		}
		if (RayDirection.y < 0)
		{
			stepY = -1;
			sideDistY = (Context.Position.y - mapY) * deltaDist.y;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0f - Context.Position.y) * deltaDist.y;
		}
		//perform DDA
		while (hit == 0)
		{
			//jump to next map square, either in x-direction, or in y-direction
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDist.x;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDist.y;
				mapY += stepY;
				side = 1;
			}
			//Check if ray has hit a wall
			if (MAPDATA[mapX + mapY * MAP_SIZE] > 0) 
				hit = 1;
		}
	
		//Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
		if (side == 0) 
			perpWallDist = (sideDistX - deltaDist.x);
		else 
			perpWallDist = (sideDistY - deltaDist.y);
	
		//Calculate height of line to draw on screen
		int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
	
		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + HALF_SCREEN_HEIGHT + pitch;
		if (drawStart < 0) 
			drawStart = 0;
		int drawEnd = lineHeight / 2 + HALF_SCREEN_HEIGHT + pitch;
		if (drawEnd >= SCREEN_HEIGHT) 
			drawEnd = SCREEN_HEIGHT - 1;
	
		//texturing calculations
		int texNum = MAPDATA[mapX + mapY * MAP_SIZE]; //1 subtracted from it so that texture 0 can be used!
	
		//calculate value of wallX
		float OwallX; //where exactly the wall was hit
		vector3 RayCollision;
		RayCollision.x = Context.Position.x + perpWallDist * RayDirection.x;
		RayCollision.y = Context.Position.y + perpWallDist * RayDirection.y;
		RayCollision.z = 0.0f;
		if (side == 0)
		{
			OwallX = Context.Position.y + perpWallDist * RayDirection.y;
		}
		else
		{
			OwallX = Context.Position.x + perpWallDist * RayDirection.x;
		}
		float wallX = OwallX-floor(OwallX);
	
		int texWidth = texture[texNum].width;
		int texHeight = texture[texNum].height;

		//x coordinate on the texture
		int texX = int(wallX * float(texWidth));
		if (side == 0 && RayDirection.x > 0) 
			texX = texWidth - texX - 1;
		if (side == 1 && RayDirection.y < 0) 
			texX = texWidth - texX - 1;
	
		// TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
		// How much to increase the texture coordinate per screen pixel
		float step = 1.0f * texHeight / lineHeight;
		// Starting texture coordinate
		float texPos = (drawStart - pitch - HALF_SCREEN_HEIGHT + lineHeight / 2) * step;
	
		float distWall = perpWallDist;
		float distPlayer = 0.0f;
	
		for (int y = drawStart; y < drawEnd; y++)
		{
			// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
			int texY = (int)texPos & (texHeight - 1);
			texPos += step;
			assert(texX < texWidth);
			assert(texY < texWidth);
			int texNum = MAPDATA[mapX + mapY * MAP_SIZE]; //1 subtracted from it so that texture 0 can be used!

			u32 color = samplepixel(texture[texNum].texture, texX, texY, texture[texNum].rowbyte);

			//texture[texNum][texHeight * texY + texX];
	

			float currentDist = SCREEN_HEIGHT / (2.0f * y - SCREEN_HEIGHT); //you could make a small lookup table for this instead
			float weight = (currentDist - distPlayer) / (distWall - distPlayer);	
			//floor
			//int floorindex = (y * SCREEN_WIDTH) + x;
	
			//buffer[y][x] = (texture[3][texWidth * floorTexY + floorTexX] >> 1) & 8355711;
			int index = (y * SCREEN_WIDTH) + x;
			int byteIndex = index / 8;
			int bitOffset = index % 8;

			//Is the pixel affected by a light
			vector3 PixelWorldPosition;
			PixelWorldPosition.x = RayCollision.x - ( texX / texture[texNum].width );
			PixelWorldPosition.y = RayCollision.y - ( texY / texture[texNum].height );// + (y / (float)SCREEN_HEIGHT); //(1.0f - mapY)/* + (float)(y / (float)SCREEN_HEIGHT)*/;
			PixelWorldPosition.z = 0.0f;
			float LightAttenuation = 0.0f;
			for(u32 i = 0; i < LightsBundle.MaxIndex; i++)
				LightAttenuation += GetLightAttenuation(PixelWorldPosition, Lights[i]);

			bool bBlack = (color == 0) || (LightAttenuation < 0.01f);

			if (bBlack)
			{
				screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
			}
			else
			{
				screen_fb[byteIndex] |= (1 << (7 - bitOffset));
			}
	
		}
		ZBuffer[x] = perpWallDist; //perpendicular distance is used
	}
}

void renderSprites()
{
	//SPRITE CASTING
	//sort sprites from far to close
#if 1
	SortSprites(SpriteOrder);
#endif
	//after sorting the sprites, do the projection and draw them
	for (u32 i = 0; i < SpritesBundle.MaxIndex; i++)
	{
		Sprite& CurrentSprite = Sprites[SpriteOrder[i]];
		if(IsIndexValid(SpritesBundle, SpriteOrder[i]))
		{
			//translate sprite position to relative to camera
			float spriteX = CurrentSprite.x - Context.Position.x;
			float spriteY = CurrentSprite.y - Context.Position.y;
	
			//transform sprite with the inverse camera matrix
			// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
			// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
			// [ planeY   dirY ]                                          [ -planeY  planeX ]
	
			float invDet = 1.0f / (Context.Plane.x * Context.Direction.y - Context.Direction.x * Context.Plane.y); //required for correct matrix multiplication
	
			float transformX = invDet * (Context.Direction.y * spriteX - Context.Direction.x * spriteY);
			float transformY = invDet * (-Context.Plane.y * spriteX + Context.Plane.x * spriteY); //this is actually the depth inside the screen, that what Z is in 3D, the distance of sprite to player, matching sqrt(spriteDistance[i])
	
			int spriteScreenX = int((SCREEN_WIDTH>> 1) * (1 + transformX / transformY));
	
			//parameters for scaling and moving the sprites
	#define vMove 0.0f
			int vMoveScreen = int(vMove / transformY) + pitch + posZ / transformY;
	
			//calculate height of the sprite on screen
			int spriteHeight = abs(int(SCREEN_HEIGHT / (transformY))) / CurrentSprite.scale; //using "transformY" instead of the real distance prevents fisheye
			//calculate lowest and highest pixel to fill in current stripe
			int drawStartY = -(spriteHeight>> 1) + (SCREEN_HEIGHT>> 1) + vMoveScreen;
			if (drawStartY < 0) drawStartY = 0;
			int drawEndY = (spriteHeight>> 1) + (SCREEN_HEIGHT>> 1) + vMoveScreen;
			if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;
	
			//calculate width of the sprite
			int spriteWidth = abs(int (SCREEN_HEIGHT / (transformY))) / CurrentSprite.scale;
			int drawStartX = -spriteWidth / 2 + spriteScreenX;
			if (drawStartX < 0) drawStartX = 0;
			int drawEndX = spriteWidth / 2 + spriteScreenX;
			if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;
	
			GameTexture& CurrentTexture = texture[CurrentSprite.texture];
			int texWidth = CurrentTexture.width;
			int texHeight = CurrentTexture.height;
	
			//loop through every vertical stripe of the sprite on screen
			for (int stripe = drawStartX; stripe < drawEndX; stripe++)
			{
				int texX = int((stripe - (-spriteWidth / 2.0f + spriteScreenX)) * texWidth / spriteWidth);
				//the conditions in the if are:
				//1) it's in front of camera plane so you don't see things behind you
				//2) it's on the screen (left)
				//3) it's on the screen (right)
				//4) ZBuffer, with perpendicular distance
				if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < ZBuffer[stripe])
				{
					for (int y = drawStartY; y < drawEndY; y++) //for every pixel of the current stripe
					{
						int d = (y-vMoveScreen) - HALF_SCREEN_HEIGHT + spriteHeight / 2;
						int texY = int((d * texHeight) / spriteHeight);
						u32 color = samplepixel(CurrentTexture.texture, texX, texY, CurrentTexture.rowbyte); //get current color from the texture
						u32 mask = samplepixel(CurrentTexture.mask, texX, texY, CurrentTexture.rowbyte); //get current color from the texture
						if (mask)
						{
							vector3 PixelWorldPosition;
							PixelWorldPosition.x = transformX;
							PixelWorldPosition.y = transformY;// + (y / (float)SCREEN_HEIGHT); //(1.0f - mapY)/* + (float)(y / (float)SCREEN_HEIGHT)*/;
							PixelWorldPosition.z = 0.0f;
							float LightAttenuation = 0.0f;
							for (u32 i = 0; i < LightsBundle.MaxIndex; i++)
								LightAttenuation += GetLightAttenuation(PixelWorldPosition, Lights[i]);
	
							int index = (y * SCREEN_WIDTH) + stripe;
							int byteIndex = index / 8;
							int bitOffset = index % 8;
							bool bBlack = (color == 0);// || (LightAttenuation < 0.01f);
							if (bBlack)
							{
								screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
							}
							else
							{
								screen_fb[byteIndex] |= (1 << (7 - bitOffset));
							}
						}
					}
				}
			}
		}
	}
}

void rotate(float rot) 
{
	const vector2 d = Context.Direction; 
	const vector2 p = Context.Plane;
	Context.Direction.x = d.x * cosf(rot) - d.y * sinf(rot);
	Context.Direction.y = d.x * sinf(rot) + d.y * cosf(rot);
	Context.Plane.x = p.x * cosf(rot) - p.y * sinf(rot);
	Context.Plane.y = p.x * sinf(rot) + p.y * cosf(rot);
}
