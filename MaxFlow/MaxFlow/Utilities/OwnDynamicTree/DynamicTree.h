#pragma once

#include "SplayTree.h"

template <class T>
class DynamicForest
{
	SplayTreeNode* nodes;
	T* inputNodes;
	uint32_t size;

private:
	SplayTreeNode* GetNode(T* node)
	{
		return nodes + (node - inputNodes);
	}

	T* GetNode(SplayTreeNode* node)
	{
		return inputNodes + (node - nodes);
	}

	void InternalAccess(SplayTreeNode* node)
	{
		uint32_t pathLength = 0;
		SplayTreeNode* firstChild = node->left;
		if (firstChild == 0)
			firstChild = node->treeParent;
		while (node->treeParent != 0)
		{
			pathLength++;
			node->UpdateModifiers();
			if (node->left == 0)
			{
				node->treeParent->MoveToRoot();
				node->treeParent->CutRight();
				node->LinkLeft(node->treeParent);
			}
			node = node->left;
		}
		SplayTreeNode* halfWay = 0;
		uint32_t halfLength = pathLength/2;
		while (node != 0)
		{
			if (halfLength-- == 0)
				halfWay = node;
			node->UpdateModifiers();
			node = node->parent;
		}
		if (pathLength >= 16 && pathLength > logl(firstChild->size))
		{
			node = firstChild->parent;
			firstChild->parent = 0;
			halfWay->MoveToRoot();
			node->left = halfWay;
			halfWay->parent = node;
		}
	}

	void Access(SplayTreeNode* node)
	{
		node->MoveToRoot();
		node->CutRight();
		InternalAccess(node);
	}

public:
	DynamicForest(T* inputNodes, uint32_t nodeCount)
	{
		this->inputNodes = inputNodes;
		nodes = new SplayTreeNode[nodeCount];
		size = sizeof(DynamicForest) + nodeCount*sizeof(SplayTreeNode);
	}

	uint32_t GetByteSize()
	{
		return size;
	}

	~DynamicForest()
	{
		delete[] nodes;
	}

	bool IsLinked(T* node)
	{
		SplayTreeNode* source = GetNode(node);
		return source->treeParent != NULL;
	}

	void Link(T* from, T* to)
	{
		SplayTreeNode* source = GetNode(from);
		SplayTreeNode* target = GetNode(to);
		if (source->treeParent == target)
			return;
		if (source->treeParent != NULL)
			Cut(from);
		Access(source);
		Access(target);
		source->LinkLeft(target);
		source->treeParent = target;
		source->childTag = target->children.AddLast(source);
	}

	void Cut(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);
		n->CutLeft();
		if (n->treeParent != 0)
		{
			n->treeParent->children.Remove(n->childTag);
			n->treeParent = 0;
			n->childTag = 0;
		}
	}

	void AddCost(T* node, int cost)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);
		n->costModifier += cost;
	}

	uint32_t GetCost(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		n->MoveToRoot();
		return n->cost;
	}

	uint32_t DebugGetCost(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		uint32_t cost = n->cost;
		while (n != NULL)
		{
			cost += n->costModifier;
			n = n->parent;
		}
		return cost;
	}

	void SetCost(T* node, uint32_t cost)
	{
		SplayTreeNode* n = GetNode(node);
		n->MoveToRoot();
		n->cost = cost;
		n->UpdateModifiers();

	}

	T* GetFirstChild(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		if (n->children.IsEmpty())
			return NULL;
		return GetNode(n->children.GetHead()->GetInstance());
	}

	T* GetSibling(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		if(n->childTag != NULL && n->childTag->GetNext() != NULL)
			return GetNode(n->childTag->GetNext()->GetInstance());
		return NULL;
	}

	T* GetLink(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		if (n->treeParent == NULL)
			return NULL;
		return GetNode(n->treeParent);
	}

	T* GetBoundingNode(T* node, uint32_t cost)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);
		while (true)
		{
			if (n->right != 0)
			{
				n->right->UpdateModifiers();
				if (n->right->minCost <= cost)
				{
					n = n->right;
					continue;
				}
			}
			if (n->treeParent != NULL && n->cost <= cost)
				break;
			if (n->left != 0)
			{				
				n->left->UpdateModifiers();
				if (n->left->minCost <= cost)
				{
					n = n->left;
					continue;
				}
			}
			return NULL;
		}
		return GetNode(n);
	}

	T* GetRoot(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);
		while (n->left != NULL)
			n = n->left;
		return GetNode(n);
	}

	T* GetMinBoundingNode(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);

		while(true)
		{
			SplayTreeNode* next = n;
			uint32_t best = n->cost;

			if (n->right != 0)
			{
				n->right->UpdateModifiers();
				if (n->right->minCost < best)
				{
					next = n->right;
					best = next->minCost;
				}
			}

			if(n->left != 0) 
			{
				n->left->UpdateModifiers();
				if (n->left->minCost <= best)
				{
					next = n->left;
					best = next->minCost;
				}
			}
			if (next == n)
				break;
			n = next;
		};
		return GetNode(n);
	}

	uint32_t GetPathLength(T* node)
	{
		SplayTreeNode* n = GetNode(node);
		Access(n);
		return n->size;
	}
};




