#pragma once
#include <stdint.h>
#include "../Graph/Graph.h"
#include <exception>

template <class T>
class Queue
{
private:
	T* ar;
	uint32_t length;
	//uint32_t m_size;
	uint32_t enqueueIndex;
	uint32_t dequeueIndex;
	T* enqueuePosition;
	T* dequeuePosition;
public:
	Queue(uint32_t size)
	{
		length = 1;
		//m_size = 0;
		while (length < size)
		{
			length<<=1;
		}
		ar = new T[length];
		enqueuePosition = ar;
		dequeuePosition = ar;
		enqueueIndex = 0;
		dequeueIndex = 0;
	}

	~Queue()
	{
		delete[] ar;
	}

	void Enqueue(T element)
	{
		enqueueIndex = (enqueueIndex+1)&(length - 1);
		if (enqueueIndex == dequeueIndex)
			throw new std::exception("Queue is full");
		*enqueuePosition = element;
		if (enqueueIndex == 0)
			enqueuePosition = ar;
		else
			enqueuePosition++;
		//m_size++;
	}
	T Dequeue()
	{
		if (enqueueIndex == dequeueIndex)
			return 0;
		dequeueIndex = (dequeueIndex+1)&(length - 1);
		T ret = *dequeuePosition;
		if (dequeueIndex == 0)
			dequeuePosition = ar;
		else
			dequeuePosition++;
		//m_size--;
		return ret;	
	}

	T PeekEnd()
	{
		if (enqueueIndex == dequeueIndex)
			return 0;
		uint32_t index = enqueueIndex;
		if (index == 0)
			index = length - 1;
		else
			index--;
		return ar[index];
	}

	uint32_t GetSize()
	{
		if (dequeueIndex <= enqueueIndex)
			return enqueueIndex - dequeueIndex;
		return length + enqueueIndex - dequeueIndex;
		//return m_size;
	}

	bool IsEmpty()
	{
		return enqueueIndex == dequeueIndex;
	}

	void Clear()
	{
		enqueuePosition = ar;
		dequeuePosition = ar;
		enqueueIndex = 0;
		dequeueIndex = 0;	
		//m_size = 0;
	}

	uint32_t GetByteSize()
	{
		return length*sizeof(T) + sizeof(Queue);
	}
};

namespace Tests
{
	void TestQueue();
}
