#pragma once

class DynamicTree;

struct DynamicTreeNode
{
private:
	int size;
	DynamicTreeNode* parent;
	double Value;

};

class DynamicTree
{
public:
	DynamicTreeNode* FindRoot(DynamicTreeNode* node);


};


