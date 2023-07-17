#pragma once

#include "geometry.h"
#include <math.h>

int getIndexOfFirstZeroBit(u64 num);

const u32 MaxEntities = 65535;
const u32 MaskCount = MaxEntities >> 6;

template<class T>
struct EntityBundle
{
	u32 MaxIndex = 0;
	T Entities[MaxEntities];
	u64 FreeListMask[MaskCount];
};

extern EntityBundle<vector2> EnemiesLocators;

template<class T>
void InitEntitiesBundle(EntityBundle<T>& InBundle);

template<class T>
bool IsIndexValid(EntityBundle<T>& InBundle, u32 InIndex)
{
	u64& Mask = InBundle.FreeListMask[InIndex >> 6];
	return (Mask & (u64(1) << InIndex));
}

template<class T>
u32 GetNextIndex(EntityBundle<T>& InBundle)
{
	u32 index = 0;
	for(int i = 0; i < MaskCount; i++)
	{
		u64& Mask = InBundle.FreeListMask[i];
		if (Mask != ~0)
		{
			index = getIndexOfFirstZeroBit(Mask);
			Mask |= u64(1) << index;
			++InBundle.MaxIndex;
			return index * (i + 1);
		}
	}

	return ~0;
}

template<class T>
void ReleaseIndex(EntityBundle<T>& InBundle, u32 InIndex)
{
	//--InBundle.EntityCount;
	u64& Mask = InBundle.FreeListMask[InIndex >> 6];
	Mask &= ~(u64(1) << InIndex);
}