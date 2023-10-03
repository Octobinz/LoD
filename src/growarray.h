#pragma once

#include "types.h"
#include <cstdlib>
#include <string.h>
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

template<class T>
struct HashNode 
{
	const char* key;
	T value;
	HashNode* next;
};

template<class T>
class HashMap {
private:
	static const int capacity = 256; // The number of buckets
	HashNode<T>* buckets[capacity];

public:
	HashMap() 
	{
		// Initialize all buckets to nullptr
		for (int i = 0; i < capacity; ++i) 
		{
			buckets[i] = nullptr;
		}
	}

	// Function to compute an 8-bit checksum from const char* content
	int hash(const char* content) 
	{
		uint8_t checksum = 0;

		// Iterate through the characters in the content
		for (int i = 0; content[i] != '\0'; i++) {
			// XOR the checksum with the ASCII value of each character
			checksum ^= content[i];
		}

		return checksum % capacity;
	}

	// Insert a key-value pair into the hashmap
	void insert(const char* key, const T& value) {
		int index = hash(key);
		HashNode<T>* newHashNode = new HashNode<T>{key, value, nullptr};

		if (buckets[index] == nullptr) {
			// If the bucket is empty, just add the new HashNode
			buckets[index] = newHashNode;
		} else {
			// If there are already HashNodes in the bucket, add the new HashNode to the front
			newHashNode->next = buckets[index];
			buckets[index] = newHashNode;
		}
	}

	// Retrieve the value associated with a given key
	T* get(const char* key) {
		int index = hash(key);
		HashNode<T>* current = buckets[index];

		while (current != nullptr) {
			if (strcmp(current->key, key) == 0) {
				return &(current->value);
			}
			current = current->next;
		}

		// If the key is not found, return nullptr
		return nullptr;
	}

	// Remove a key-value pair from the hashmap
	void remove(const char* key) {
		int index = hash(key);
		HashNode<T>* current = buckets[index];
		HashNode<T>* prev = nullptr;

		while (current != nullptr) {
			if (strcmp(current->key, key) == 0) {
				if (prev != nullptr) {
					// If the HashNode to remove is not the first in the list
					prev->next = current->next;
				} else {
					// If the HashNode to remove is the first in the list
					buckets[index] = current->next;
				}
				delete current;
				return;
			}
			prev = current;
			current = current->next;
		}
	}

	// Destructor to free memory
	~HashMap() {
		for (int i = 0; i < capacity; ++i) {
			HashNode<T>* current = buckets[i];
			while (current != nullptr) {
				HashNode<T>* next = current->next;
				delete current;
				current = next;
			}
		}
	}
};
