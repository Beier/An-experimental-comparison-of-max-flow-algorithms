#pragma once

#include <stdint.h>
#include "../LinkedList.h"


class SplayTreeNode
{
public:
	SplayTreeNode* parent;
	SplayTreeNode* left;
	SplayTreeNode* right;

	
	SplayTreeNode* treeParent;

	LinkedList<SplayTreeNode*> children;
	LinkedList<SplayTreeNode*>::Node* childTag;
	


	uint32_t cost;
	int32_t costModifier;

	uint32_t minCost;

	uint32_t size;

	uint32_t GetMinCost()
	{
		return minCost + costModifier;
	}

	SplayTreeNode()
	{
		parent = 0;
		left = 0;
		right = 0;
		cost = 0;
		costModifier = 0;
		treeParent = 0;
		childTag = 0;
		minCost = 0;
		size = 1;
	}


	void LinkRight(SplayTreeNode* node)
	{
		UpdateModifiers();
		right = node;
		node->parent = this;
		UpdateModifiers();
	}

	void CutRight()
	{
		UpdateModifiers();
		if (right != 0)
			right->parent = 0;
		right = 0;
		UpdateModifiers();
	}
	void LinkLeft(SplayTreeNode* node)
	{
		UpdateModifiers();
		left = node;
		node->parent = this;
		UpdateModifiers();
	}
	void CutLeft()
	{
		UpdateModifiers();
		if (left != 0)
			left->parent = 0;
		left = 0;
		UpdateModifiers();
	}

	

	void UpdateModifiers()
	{
		MoveModifiersDown();
		minCost = UINT32_MAX;
		size = 1;
		if (treeParent != NULL)
			minCost = cost;
		if (left != 0)
		{
			if (left->treeParent != NULL && left->GetMinCost() < minCost)
				minCost = left->GetMinCost();
			size += left->size;
		}
		if (right != 0)
		{
			if (right->treeParent != NULL && right->GetMinCost() < minCost)
				minCost = right->GetMinCost();
			size += right->size;
		}
	}

private:
	void MoveModifiersDown()
	{
		if (costModifier != 0)
		{
			if (left != 0)
				left->costModifier += costModifier;
			if (right != 0)
				right->costModifier += costModifier;
			if (treeParent != NULL)
			{
				cost += costModifier;
			}
			costModifier = 0;
		}
	}

	void RotateLeft()
	{
		SplayTreeNode* r = right;
		MoveModifiersDown();
		r->MoveModifiersDown();		
		right = r->left;
		if (right != 0)
			right->parent = this;
		r->left = this;
		if (parent != 0)
		{
			if (parent->left == this)
				parent->left = r;
			else
				parent->right = r;
		}
		r->parent = parent;
		parent = r;
		UpdateModifiers();
		r->UpdateModifiers();
	}

	void RotateRight()
	{
		SplayTreeNode* l = left;
		MoveModifiersDown();
		l->MoveModifiersDown();		
		left = l->right;
		if (left != 0)
			left->parent = this;
		l->right = this;
		if (parent != 0)
		{
			if (parent->left == this)
				parent->left = l;
			else
				parent->right = l;
		}
		l->parent = parent;
		parent = l;
		UpdateModifiers();
		l->UpdateModifiers();
	}

public:

	void MoveToRoot()
	{
		while (true)
		{
			UpdateModifiers();
			SplayTreeNode* p = parent;
			if (p == 0)
				return;
			if (p->parent == 0)
			{
				if (this == p->left)
					p->RotateRight();
				else
					p->RotateLeft();
			}
			else if ((this == p->left) == (p == p->parent->left))
			{
				SplayTreeNode* gp = p->parent;
				if (this == p->left)
				{
					gp->RotateRight();
					p->RotateRight();
				}
				else
				{
					gp->RotateLeft();
					p->RotateLeft();
				}
			}
			else
			{
				SplayTreeNode* gp = p->parent;
				if (this == p->left)
				{
					p->RotateRight();
					gp->RotateLeft();
				}
				else
				{
					p->RotateLeft();
					gp->RotateRight();
				}
			}
		}
	}


};