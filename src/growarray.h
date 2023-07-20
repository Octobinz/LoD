#pragma once

#include "types.h"
#include <utility>
#include <memory>
#include <cstring>

template<class T>
class GrowArray
{
public:
	GrowArray() = default;

	void Preallocate(size_t ElementCount)
	{
		ScratchMemory = std::make_unique<T[]>(ElementCount);
		m_MaxCount = ElementCount;
	}

	void AddElement(const T& In)
	{
		if (m_Count >= m_MaxCount)
		{
			m_MaxCount = m_MaxCount == 0 ? 1 : (m_MaxCount << 1);
			auto NewScratchMemory = std::make_unique<T[]>(m_MaxCount);
			std::memcpy(NewScratchMemory.get(), ScratchMemory.get(), m_Count * sizeof(T));
			ScratchMemory = std::move(NewScratchMemory);
		}

		ScratchMemory[m_Count++] = In;
	}

	bool IsEmpty() const
	{
		return m_Count == 0;
	}

	T& Get(size_t Index)
	{
		return ScratchMemory[Index];
	}

	const T& Get(size_t Index) const
	{
		return ScratchMemory[Index];
	}

	T& Front()
	{
		return ScratchMemory[0];
	}

	const T& Front() const
	{
		return ScratchMemory[0];
	}

	void Pop()
	{
		if (m_Count == 0)
			return;

		--m_Count;
		if (m_Count > 0)
			std::memmove(ScratchMemory.get(), ScratchMemory.get() + 1, m_Count * sizeof(T));
	}

	void Deallocate()
	{
		m_MaxCount = 0;
		ScratchMemory.reset();
	}

	size_t Count() const
	{
		return m_Count;
	}

private:
	std::unique_ptr<T[]> ScratchMemory = nullptr;
	size_t m_Count = 0;
	size_t m_MaxCount = 0;
};