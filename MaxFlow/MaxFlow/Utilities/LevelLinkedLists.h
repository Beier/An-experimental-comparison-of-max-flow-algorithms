

#pragma once
#include "LinkedList.h"
#include "../Graph/Graph.h"

class LevelLinkedLists
{
private:
	LinkedList<Node*>* linkedLists;
	uint32_t levelCount;
	uint32_t lowestNonEmptyList;
	uint32_t highestNonEmptyList;
	uint32_t count;


	void UpdateLevelPointers(uint32_t removedLevel)
	{
		if (count == 0)
		{
			lowestNonEmptyList = levelCount - 1;
			highestNonEmptyList = 0;
		}
		else
		{
			if (removedLevel == lowestNonEmptyList)
			{
				for (LinkedList<Node*>* list = linkedLists + removedLevel; list->IsEmpty(); list++)
					lowestNonEmptyList++;				
			}
			if (removedLevel == highestNonEmptyList)
			{
				for (LinkedList<Node*>* list = linkedLists + removedLevel; list->IsEmpty(); list--)
					highestNonEmptyList--;				
			}
		}
	}

public:

	LevelLinkedLists(uint32_t n)
	{
		levelCount = 2*n-1;
		linkedLists = new LinkedList<Node*>[levelCount];
		count = 0;
		lowestNonEmptyList = levelCount - 1;
		highestNonEmptyList = 0;
	}

	~LevelLinkedLists()
	{
		delete[] linkedLists;
	}

	LinkedList<Node*>::Node* Insert(Node* node, uint32_t level)
	{
		if (lowestNonEmptyList > level)
			lowestNonEmptyList = level;
		if (highestNonEmptyList < level)
			highestNonEmptyList = level;
		count++;
		return linkedLists[level].AddFirst(node);
	}

	void Remove(LinkedList<Node*>::Node* key, uint32_t level)
	{
		linkedLists[level].Remove(key);
		count--;
		UpdateLevelPointers(level);
	}

	void Move(LinkedList<Node*>::Node* key, uint32_t oldLevel, uint32_t newLevel)
	{
		linkedLists[newLevel].MoveFirst(linkedLists + oldLevel, key);
		
		if (lowestNonEmptyList > newLevel)
			lowestNonEmptyList = newLevel;
		if (highestNonEmptyList < newLevel)
			highestNonEmptyList = newLevel;
		UpdateLevelPointers(oldLevel);
	}

	Node* GetLowestLevelNode()
	{
		return linkedLists[lowestNonEmptyList].GetHead()->GetInstance();
	}

	Node* GetHighestLevelNode()
	{
		return linkedLists[highestNonEmptyList].GetHead()->GetInstance();
	}

	uint32_t GetCount()
	{
		return count;
	}




};
