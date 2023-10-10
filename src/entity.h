#pragma once

#include "geometry.h"
#include <math.h>

const u32 MaxEntities = 256;
const u32 MaskCount = MaxEntities >> 5;

#ifdef _MSC_VER

#include <intrin.h>
static FORCEINLINE int getIndexOfFirstZeroBit(u32 num)
{
	unsigned long index = 0;
	unsigned char isNonzero = _BitScanReverse(&index, num);
	return isNonzero ? index + 1 : 0;
}
#else

static FORCEINLINE int getIndexOfFirstZeroBit(uint32_t num)
{
	if (num == 0)
	{
		return 0;
	}
	return 31 - __builtin_clz(num) + 1;
}
#endif


template<class T>
struct EntityBundle
{
	EntityBundle()
	{
		Entities = new T[MaxEntities]();
	}
	~EntityBundle()
	{
		delete[] Entities;
	}
	u32 MaxIndex = 0;
	T* Entities;
	u32 FreeListMask[MaskCount];
};

extern EntityBundle<vector2> EnemiesLocators;

template<class T>
void InitEntitiesBundle(EntityBundle<T>& InBundle);

template<class T>
FORCEINLINE bool IsIndexValid(EntityBundle<T>& InBundle, u32 InIndex)
{
	const u32& Mask = InBundle.FreeListMask[InIndex >> 5];
	return (Mask & (u32(1) << InIndex));
}

extern PlaydateAPI* pd;

template<class T>
FORCEINLINE u32 GetNextIndex(EntityBundle<T>& InBundle)
{
	for(u32 i = 0; i < MaskCount; i++)
	{
		u32& Mask = InBundle.FreeListMask[i];
		if (Mask != u32(~0))
		{
			u32 index = getIndexOfFirstZeroBit(Mask);
			Mask |= u32(1) << index;
			++InBundle.MaxIndex;
			return index * (i + 1);
		}
	}

	return ~0;
}

template<class T>
FORCEINLINE void ReleaseIndex(EntityBundle<T>& InBundle, u32 InIndex)
{
	//--InBundle.EntityCount;
	u32& Mask = InBundle.FreeListMask[InIndex >> 5];
	Mask &= ~(u32(1) << InIndex);
}