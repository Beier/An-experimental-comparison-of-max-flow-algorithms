

#pragma once
#include <iostream>
#include <stdint.h>



class LinkedListNodeManager;



template <class T>
class LinkedList
{
public:
	struct Node
	{
		Node* n;
		Node* p;
		T i;
		LinkedList* parent;
		
		Node* GetNext()
		{
			return n;
		}
	
		Node* GetPrevious()
		{
			return p;
		}
	
		T GetInstance()
		{
			return i;
		}
	};
private:
	
	Node* head;
	Node* tail;

	static Node** unusedNodes;
	static int unusedNodesLength;
	static int unusedNodesCount;

	
	
public:
	LinkedList()
	{
		head = 0;
		tail = 0;
		count = 0;
	}

	~LinkedList()
	{
		while (!IsEmpty())
			Remove(head);
	}

	static void Allocate(uint32_t count)
	{
		if (count <= LinkedListNodeManager::GetAvalibleCount())
			return;
		count -= LinkedListNodeManager::GetAvalibleCount();
		LinkedListNodeManager::BulkAllocate<Node>(count);
	}

	Node* GetHead()
	{
		return head;
	}

	Node* GetTail()
	{
		return tail;
	}



	uint32_t GetCount()
	{
		return count;
	}

	Node* AddFirst(T item)
	{
		Node* node = LinkedListNodeManager::GetNewNode<Node>();
		node->i = item;
		node->n = head;
		node->p = 0;
		node->parent = this;
		if (head != 0)
			head->p = node;
		head = node;
		if (tail == 0)
			tail = node;
		count++;
		return node;
	}

	void MoveFirst(LinkedList<T>* originalList, Node* node)
	{
		originalList->Cut(node);
		node->parent = this;
		node->n = head;
		node->p = 0;
		if (head != 0)
			head->p = node;
		head = node;
		if (tail == 0)
			tail = node;
		originalList->count--;
		count++;
	}
	
	Node* AddLast(T item)
	{
		Node* node = LinkedListNodeManager::GetNewNode<Node>();
		node->i = item;
		node->n = 0;
		node->p = tail;
		node->parent = this;
		if (tail != 0)
			tail->n = node;
		tail = node;
		if (head == 0)
			head = tail;
		count++;
		return node;
	}

	void MoveLast(LinkedList<T>* originalList, Node* node)
	{
		node->parent = this;
		node->n = 0;
		node->p = tail;
		if (tail != 0)
			tail->n = node;
		tail = node;
		if (head == 0)
			head = node;
		originalList->count--;
		count++;
	}
	

	void Remove(Node* node)
	{
		Cut(node);
		count--;
		node->parent = 0;
		LinkedListNodeManager::FreeNode<Node>(node);
	}

	T RemoveFirst()
	{
		Node* node = head;
		T instance = node->i;
		head = node->n;
		if (head == 0)
			tail = 0;
		else
			head->p = 0;
		node->parent = 0;
		LinkedListNodeManager::FreeNode<Node>(node);
		count--;
		return instance;
	}

	T RemoveLast()
	{
		Node* node = tail;
		T instance = node->i;
		tail = node->p;
		if (tail == 0)
			head = 0;
		else
			tail->n = 0;
		node->parent = 0;
		LinkedListNodeManager::FreeNode<Node>(node);
		count--;
		return instance;
	}

	bool IsEmpty()
	{
		return head == 0;
	}

	bool Contains(Node* node)
	{
		return node->parent == this;
	}

	bool Contains(T* node)
	{
		Node* curr = NULL;
		Node* next = head;
		while (next != NULL)
		{
			curr = next;
			next = next->n;

			if (curr->i == node)
				return true;
		}
		return false;
	}

	void Clear()
	{
		while (count > 0)
		{
			RemoveFirst();
		}
	}
	
private:
	uint32_t count;
	
	void Cut(Node* node)
	{
		if (node->n != 0)
			node->n->p = node->p;
		if (node->p != 0)
			node->p->n = node->n;
		if (node == head)
			head = node->n;
		if (node == tail)
			tail = node->p;
	}
};



class LinkedListNodeManager
{
	struct BulkAllocation
	{
		LinkedList<void*>::Node* pointer;
		size_t size;
	};

	static LinkedList<void*>::Node** unusedNodes;
	static int unusedNodesLength;
	static int unusedNodesCount;

	static BulkAllocation* bulkAllocations;
	static int bulkAllocationsLength;
	static int bulkAllocationsCount;
public:
	template <class T>
	static T* GetNewNode()
	{
		if (sizeof(T) != sizeof(LinkedList<void*>::Node))
			throw "Error";
		if (unusedNodesCount == 0)
			return new T();
		return (T*)unusedNodes[--unusedNodesCount];
	}
	
	template <class T>
	static void FreeNode(T* node)
	{
		if (sizeof(T) != sizeof(LinkedList<void*>::Node))
			throw "Error";
		if (unusedNodesCount == unusedNodesLength)
		{
			LinkedList<void*>::Node** newArray = new LinkedList<void*>::Node*[unusedNodesLength*2];
			LinkedList<void*>::Node** pOld = unusedNodes;
			LinkedList<void*>::Node** pNew = newArray;
			for (int i = 0; i < unusedNodesLength; i++)
				*pNew++ = *pOld++;
			delete[] unusedNodes;
			unusedNodes = newArray;
			unusedNodesLength *= 2;
		}
		unusedNodes[unusedNodesCount++] = (LinkedList<void*>::Node*) node;
	}
	
	template <class T>
	static void BulkAllocate(size_t count)
	{
		if (sizeof(T) != sizeof(LinkedList<void*>::Node))
			throw "Error";
		if (bulkAllocationsCount == bulkAllocationsLength)
		{
			BulkAllocation* newAr = new BulkAllocation[bulkAllocationsLength*2];
			for (int i = 0; i < bulkAllocationsLength; i++)
				bulkAllocations[i] = newAr[i];
			bulkAllocationsLength *= 2;
		}

		LinkedList<void*>::Node* nodes = new LinkedList<void*>::Node[count];
		for (int i = 0; i < count; i++)
			LinkedListNodeManager::FreeNode(nodes + i);
		bulkAllocations[bulkAllocationsCount].pointer = nodes;
		bulkAllocations[bulkAllocationsCount].size = count;
		bulkAllocationsCount++;
	}

	static uint32_t GetAvalibleCount()
	{
		return unusedNodesCount;
	}

	static void ClearCache()
	{
		//printf("Removing single allocations\n");
		for (int i = 0; i < unusedNodesCount; i++)
		{
			//std::cout << i << "/" << unusedNodesCount << "\n";
			LinkedList<void*>::Node* p = unusedNodes[i];
			bool isFromBulk = false;
			for (int j = 0; j < bulkAllocationsCount; j++)
			{
				if (p >= bulkAllocations[j].pointer && p < (bulkAllocations[j].pointer + bulkAllocations[j].size))
				{
					//std::cout << "Is from bulk\n";
					isFromBulk = true;
					break;
				}
			}
			if (!isFromBulk)
			{
				//std::cout << "Not from bulk\n";
				delete p;
			}
		}
		//printf("Removing bulk allocations\n");
		for (int i = 0; i < bulkAllocationsCount; i++)
			delete[] bulkAllocations[i].pointer;
		
		//printf("Done\n");
		bulkAllocationsCount = 0;
		unusedNodesCount = 0;	

		delete[] unusedNodes;
		unusedNodes = new LinkedList<void*>::Node*[1024];
		unusedNodesLength = 1024;
		unusedNodesCount = 0;

		delete[] bulkAllocations;
		bulkAllocations = new BulkAllocation[1];
		bulkAllocationsLength = 1;
		bulkAllocationsCount = 0;
	
	}

};