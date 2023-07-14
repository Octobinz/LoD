#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <vector>
#include <tchar.h>
#include <algorithm>

extern "C"
{
#include "pd_api.h"
}
#include "geometry.h"
#include "gameobject.h"

static uint8_t *screen_fb = NULL;		// frame buffer

static PlaydateAPI* gpd;

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


float ZBuffer[SCREEN_WIDTH];

#define numSprites 19
#define numLights 2

Light lights[numLights] = 
{
	{1.0f, 1.0f,1.0f, 2.5f},
	{8.0f, 8.0f,1.0f, 2.5f},
};

Sprite StaticSprites[numSprites] =
{
	{20.5f, 11.5f, 10}, //green light in front of playerstart
	//green lights in every room
	{18.5f,4.5f, 10},
	{10.0f,4.5f, 10},
	{10.0f,12.5f,10},
	{3.5f, 6.5f, 10},
	{3.5f, 20.5f,10},
	{3.5f, 14.5f,10},
	{14.5f,20.5f,10},

	//row of pillars in front of wall: fisheye test
	{18.5f, 10.5f, 9},
	{18.5f, 11.5f, 9},
	{18.5f, 12.5f, 9},

	//some barrels around the map
	{21.5f, 1.5f, 8},
	{15.5f, 1.5f, 8},
	{16.0f, 1.8f, 8},
	{16.2f, 1.2f, 8},
	{3.5f,  2.5f, 8},
	{9.5f, 15.5f, 8},
	{10.0f, 15.1f,8},
	{10.5f, 15.8f,8},
};
//arrays used to sort the sprites
int spriteOrder[numSprites];
float spriteDistance[numSprites];

//function used to sort the sprites
//sort the sprites based on distance
void sortSprites(int* order, float* dist, int amount)
{
	std::vector<std::pair<float, int>> sprites(amount);
	for(int i = 0; i < amount; i++) 
	{
		sprites[i].first = dist[i];
		sprites[i].second = order[i];
	}
	std::sort(sprites.begin(), sprites.end());
	// restore in reverse order to go from farthest to nearest
	for(int i = 0; i < amount; i++) 
	{
		dist[i] = sprites[amount - i - 1].first;
		order[i] = sprites[amount - i - 1].second;
	}
}

std::vector<GameTexture> texture;

PlayerContext Context;

float GetLightAttenuation(vector3& InPosition, Light& InLight)
{
	const vector3 v = vector3(InPosition.x - InLight.location.x, InPosition.y - InLight.location.y, InPosition.z - InLight.location.z);
	float Distance = (vector_length(v) + 0.1f);
	float ratio =  std::max(1.0f - (Distance / InLight.radius), 0.0f);
	return ratio;
}


#define samplepixel(data, x, y, rowbytes) (((data[(y)*rowbytes+(x)/8] & (1 << (uint8_t)(7 - ((x) % 8)))) != 0) ? kColorWhite : kColorBlack)

#define MAP_SIZE 24

static u8 MAPDATA[MAP_SIZE * MAP_SIZE] =
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

static void renderFloor()
{
	//FLOOR CASTING
	for(int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		// whether this section is floor or ceiling
		bool is_floor = y > SCREEN_HEIGHT / 2 + pitch;

		// rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
		float rayDirX0 = Context.Direction.x - Context.Plane.x;
		float rayDirY0 = Context.Direction.y - Context.Plane.y;
		float rayDirX1 = Context.Direction.x + Context.Plane.x;
		float rayDirY1 = Context.Direction.y + Context.Plane.y;

		// Current y position compared to the center of the screen (the horizon)
		int p = is_floor ? (y - SCREEN_HEIGHT / 2 - pitch) : (SCREEN_HEIGHT / 2 - y + pitch);

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
			for(int i = 0; i < numLights; i++)
				LightAttenuation += GetLightAttenuation(PixelWorldPosition, lights[i]);

			if(is_floor) 
			{
				// floor
				color = samplepixel(texture[floorTexture].texture, tx, ty, texture[floorTexture].rowbyte);
				int index = (y * SCREEN_WIDTH) + x;
				int byteIndex = index / 8;
				int bitOffset = index % 8;
	
				bool bBlack = (color == 0) || LightAttenuation < 0.01f;

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

				bool bBlack = (color == 0) || LightAttenuation < 0.01f;

				if (bBlack)
				{
					screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
				}
				else
				{
					if (LightAttenuation < 0.99f/* && (tx&1 || ty&1)*/)
					{
						screen_fb[byteIndex] |= (1 << (7 - bitOffset));
					}
					else
					{
						screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
					}				
				}			
			}
		}
	}
}

static void renderWalls()
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
		int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2 + pitch;
		if (drawStart < 0) 
			drawStart = 0;
		int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2 + pitch;
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
		float texPos = (drawStart - pitch - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;
	
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
			bool bBlack = (color == 0);

			//Is the pixel affected by a light
			vector3 PixelWorldPosition;
			PixelWorldPosition.x = RayCollision.x;
			PixelWorldPosition.y = RayCollision.y;// + (y / (float)SCREEN_HEIGHT); //(1.0f - mapY)/* + (float)(y / (float)SCREEN_HEIGHT)*/;
			PixelWorldPosition.z = 0.0f;
			float LightAttenuation = GetLightAttenuation(PixelWorldPosition, lights[0]);
			if (bBlack)
			{
				screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
			}
			else
			{
				if (LightAttenuation > 0.01f)
				{
					screen_fb[byteIndex] |= (1 << (7 - bitOffset));
				}
				else
				{
					screen_fb[byteIndex] &= ~(1 << (7 - bitOffset));
				}
			}
	
		}
		ZBuffer[x] = perpWallDist; //perpendicular distance is used
	}
}

static void renderSprites()
{
	//SPRITE CASTING
	//sort sprites from far to close
	for(int i = 0; i < numSprites; i++)
	{
		spriteOrder[i] = i;
		spriteDistance[i] = ((Context.Position.x - StaticSprites[i].x) * (Context.Position.x - StaticSprites[i].x) + (Context.Position.y - StaticSprites[i].y) * (Context.Position.y - StaticSprites[i].y)); //sqrt not taken, unneeded
	}
	sortSprites(spriteOrder, spriteDistance, numSprites);

	//after sorting the sprites, do the projection and draw them
	for (int i = 0; i < numSprites; i++)
	{
		//translate sprite position to relative to camera
		float spriteX = StaticSprites[spriteOrder[i]].x - Context.Position.x;
		float spriteY = StaticSprites[spriteOrder[i]].y - Context.Position.y;

		//transform sprite with the inverse camera matrix
		// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
		// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
		// [ planeY   dirY ]                                          [ -planeY  planeX ]

		float invDet = 1.0f / (Context.Plane.x * Context.Direction.y - Context.Direction.x * Context.Plane.y); //required for correct matrix multiplication

		float transformX = invDet * (Context.Direction.y * spriteX - Context.Direction.x * spriteY);
		float transformY = invDet * (-Context.Plane.y * spriteX + Context.Plane.x * spriteY); //this is actually the depth inside the screen, that what Z is in 3D, the distance of sprite to player, matching sqrt(spriteDistance[i])

		int spriteScreenX = int((SCREEN_WIDTH>> 1) * (1 + transformX / transformY));

		//parameters for scaling and moving the sprites
#define uDiv 1
#define vDiv 1
#define vMove 0.0f
		int vMoveScreen = int(vMove / transformY) + pitch + posZ / transformY;

		//calculate height of the sprite on screen
		int spriteHeight = abs(int(SCREEN_HEIGHT / (transformY))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
		//calculate lowest and highest pixel to fill in current stripe
		int drawStartY = -(spriteHeight>> 1) + (SCREEN_HEIGHT>> 1) + vMoveScreen;
		if (drawStartY < 0) drawStartY = 0;
		int drawEndY = (spriteHeight>> 1) + (SCREEN_HEIGHT>> 1) + vMoveScreen;
		if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

		//calculate width of the sprite
		int spriteWidth = abs(int (SCREEN_HEIGHT / (transformY))) / uDiv;
		int drawStartX = -spriteWidth / 2 + spriteScreenX;
		if (drawStartX < 0) drawStartX = 0;
		int drawEndX = spriteWidth / 2 + spriteScreenX;
		if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

		int texWidth = texture[StaticSprites[spriteOrder[i]].texture].width;
		int texHeight = texture[StaticSprites[spriteOrder[i]].texture].height;

		//loop through every vertical stripe of the sprite on screen
		for(int stripe = drawStartX; stripe < drawEndX; stripe++)
		{
			int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
			//the conditions in the if are:
			//1) it's in front of camera plane so you don't see things behind you
			//2) it's on the screen (left)
			//3) it's on the screen (right)
			//4) ZBuffer, with perpendicular distance
			if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < ZBuffer[stripe])
			{
				for (int y = drawStartY; y < drawEndY; y++) //for every pixel of the current stripe
				{
					int d = (y-vMoveScreen) * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128; //256 and 128 factors to avoid floats
					int texY = ((d * texHeight) / spriteHeight) / 256;
					u32 color = samplepixel(texture[StaticSprites[spriteOrder[i]].texture].texture, texX, texY, texture[StaticSprites[spriteOrder[i]].texture].rowbyte); //get current color from the texture
					u32 mask = samplepixel(texture[StaticSprites[spriteOrder[i]].texture].mask, texX, texY, texture[StaticSprites[spriteOrder[i]].texture].rowbyte); //get current color from the texture
					if(mask) 
					{
						vector3 PixelWorldPosition;
						PixelWorldPosition.x = transformX;
						PixelWorldPosition.y = transformY;// + (y / (float)SCREEN_HEIGHT); //(1.0f - mapY)/* + (float)(y / (float)SCREEN_HEIGHT)*/;
						PixelWorldPosition.z = 0.0f;
						float LightAttenuation = GetLightAttenuation(PixelWorldPosition, lights[0]);

						int index = (y * SCREEN_WIDTH) + stripe;
						int byteIndex = index / 8;
						int bitOffset = index % 8;
						bool bBlack = color == 0;
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

static void render() 
{
	renderFloor();
	renderWalls();
	renderSprites();
}

static void rotate(float rot) 
{
	const vector2 d = Context.Direction; 
	const vector2 p = Context.Plane;
	Context.Direction.x = d.x * cosf(rot) - d.y * sinf(rot);
	Context.Direction.y = d.x * sinf(rot) + d.y * cosf(rot);
	Context.Plane.x = p.x * cosf(rot) - p.y * sinf(rot);
	Context.Plane.y = p.x * sinf(rot) + p.y * cosf(rot);
}

static LCDBitmap *loadImageAtPath(PlaydateAPI* api, const char *path)
{
	const char *outErr = NULL;
	LCDBitmap *img = api->graphics->loadBitmap(path, &outErr);
	if ( outErr != NULL ) {
		api->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
	}
	return img;
}

static int LoadTexture(PlaydateAPI* api, const char* filename)
{
	const char *outErr = NULL;
	LCDBitmap *img = api->graphics->loadBitmap(filename, &outErr);
		
	int width; 
	int height; 
	int rowbytes; 
	uint8_t* mask; 
	uint8_t* data; 
	api->graphics->getBitmapData(img, &width, &height, &rowbytes, &mask, &data);
	int index = texture.size();
	texture.push_back(GameTexture());

	texture[index].texture = (u8*)malloc(width * height);
	texture[index].mask = (u8*)malloc(width * height);
	texture[index].width = width;
	texture[index].height = height;
	texture[index].rowbyte = rowbytes;
	texture[index].img = img;
	memcpy(texture[index].texture, data, (width*height) / rowbytes);
	
	if (mask != nullptr)
	{
		memcpy(texture[index].mask, mask, 512);
	}
	return index;
}

void InitPlayer(PlaydateAPI* pd)
{
	Context.SwordSprite.texture = LoadTexture(pd, "textures/player_sword_1.png");
	Context.ShieldSprite.texture = LoadTexture(pd, "textures/player_shield_1.png");
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
	int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if (event == kEventInit)
	{
		gpd = pd;
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if (font == NULL)
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);
		pd->display->setRefreshRate(50);
		pd->display->setScale(1);		

		LoadTexture(pd, "textures/eagle.png");
		LoadTexture(pd, "textures/blank_rectangle.png");
		LoadTexture(pd, "textures/purplestone.png");
		LoadTexture(pd, "textures/greystone.png");
		LoadTexture(pd, "textures/bluestone.png");
		LoadTexture(pd, "textures/mossy.png");
		LoadTexture(pd, "textures/wood.png");
		LoadTexture(pd, "textures/colorstone.png");
		LoadTexture(pd, "textures/barrel.png");
		LoadTexture(pd, "textures/pillar.png");
		LoadTexture(pd, "textures/greenlight.png");
		LoadTexture(pd, "textures/woodenfloor.png");
		LoadTexture(pd, "textures/greystone.png");

		InitPlayer(pd);
		uint8_t* pd_screen = pd->graphics->getFrame();
		screen_fb = pd_screen;

		Context.Position.x = 5; 
		Context.Position.y = 5;
		vector2 v;
		v.x = -1.0f;
		v.y = 0.1f;
		vector_normalize(v);
		Context.Direction = v;
		Context.Plane.x = 0.0f; 
		Context.Plane.y = 0.66f;
	}
	
	return 0;
}

static int offset = 0;
static int update(void* userdata)
{
	PlaydateAPI* pd = (PlaydateAPI *)userdata;


	pd->graphics->setFont(font);
	pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, 0, 0);
	pd->graphics->markUpdatedRows(0, 240 - 1);
	gpd->graphics->clear(kColorWhite);

	uint8_t* pd_screen = gpd->graphics->getFrame();
	PDButtons current;
	pd->system->getButtonState(&current, nullptr, nullptr);
	const float rotspeed = 3.0f * 0.016f,
	movespeed = 3.0f * 0.016f;

	if (current & kButtonLeft) 
	{
		rotate(+rotspeed);
	}

	if (current & kButtonRight) 
	{
		rotate(-rotspeed);
	}


	if (current & kButtonUp) 
	{

		float futurex = Context.Position.x + Context.Direction.x * movespeed;
		float futurey = Context.Position.y + Context.Direction.y * movespeed;
		if(MAPDATA[int(futurex) + int(Context.Position.y) * MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		//if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		if (MAPDATA[int(Context.Position.x) + int(futurey) * MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}

	if (current & kButtonDown) 
	{

		float futurex = Context.Position.x - Context.Direction.x * movespeed;
		float futurey = Context.Position.y - Context.Direction.y * movespeed;

		if(MAPDATA[int(futurex) + int(Context.Position.y)* MAP_SIZE] == 0)
		{
			Context.Position.x = futurex;
		}
		if (MAPDATA[int(Context.Position.x) + int(futurey)* MAP_SIZE] == 0)
		{
			Context.Position.y = futurey;
		}
	}

	if (current & kButtonB) {
		//pitch += 10;
		lights[0].radius -= 0.1f;
	}
	if (current & kButtonA) {
		//		pitch -= 10;
		lights[0].radius += 0.1f;
	}

	render();
	GameTexture& SwordTexture = texture[Context.SwordSprite.texture];
	pd->graphics->drawBitmap(SwordTexture.img, 250, 0, kBitmapUnflipped);
	pd->system->drawFPS(0, 0);
	return 1;
}

