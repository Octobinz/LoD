#pragma once

#include "types.h"
#include <utility>

template<class T>
class GrowArray
{
public:

	FORCEINLINE void Preallocate(u32 ElementCount)
	{
		ScratchMemory = malloc(ElementCount * sizeof(T));
		for(u32 i = 0; i < ElementCount; i++)
		{
			new((T*)(ScratchMemory) + i) T();
		}
		m_Count = ElementCount;
		m_MaxCount = ElementCount;
	}
	FORCEINLINE void push_back(T& In)
	{
		AddElement(In);
	}
	
	FORCEINLINE void AddElement(T& In)
	{
		if(m_Count >= m_MaxCount)
		{
			m_MaxCount = m_MaxCount == 0 ? 1 : (m_MaxCount << 1);
			void* NewScratchMemory = malloc(m_MaxCount * sizeof(T));
			memcpy(NewScratchMemory, ScratchMemory, m_Count * sizeof(T));
			ScratchMemory = std::move(NewScratchMemory);
		}

		static_cast<T*>(ScratchMemory)[m_Count++] = In;
	}
	FORCEINLINE bool empty()
	{
		return m_Count == 0;
	}
	FORCEINLINE T& Get(u32 Index)
	{
		return static_cast<T*>(ScratchMemory)[Index];
	}

	FORCEINLINE T& operator [](const int & index)
	{
		return Get(index);
	}

	FORCEINLINE T& Front()
	{
		return static_cast<T*>(ScratchMemory)[0];
	}

	FORCEINLINE void erase(int index)
	{
		--m_Count;
		if(m_Count == 0)
		{
			Deallocate();
		}
		else
		{
			memcpy((T*)ScratchMemory + index, (T*)(ScratchMemory) + 1, (m_Count - index ) * sizeof(T));
		}
	}

	FORCEINLINE void Pop()
	{
		--m_Count;
		if(m_Count == 0)
		{
			Deallocate();
		}
		else
		{
			memcpy(ScratchMemory, (T*)(ScratchMemory) + 1, m_Count * sizeof(T));
		}
	}
	FORCEINLINE void Deallocate()
	{
		m_MaxCount = 0;
		if(ScratchMemory != nullptr)
			free(ScratchMemory);
		ScratchMemory = nullptr;
	}

	FORCEINLINE void Reset()
	{
		m_Count = 0;
	}

	~GrowArray()
	{
		Deallocate();
	}
	FORCEINLINE u32 size() { return m_Count; }
private:
	void* ScratchMemory = nullptr;
	u32 m_Count = 0;
	u32 m_Front = 0;
	u32 m_MaxCount = 0;
};