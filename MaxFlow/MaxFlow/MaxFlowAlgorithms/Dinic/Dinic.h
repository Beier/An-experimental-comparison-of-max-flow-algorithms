#pragma once
#include "../MaxFlowAlgorithm.h"
#include <stack>
#include "../../Utilities/Queue.h"

class Dinic : public IMaxFlowAlgorithm
{
public:
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);

private:	
	struct DinicTag
	{
		uint32_t Distance;
		uint32_t OriginalEdgeCount;
		Edge* pBack;
		Edge* currentEdge;
	};


	Graph* graph;
	Node* source;
	Node* target;
	Queue<Node*>* queue;
	DinicTag* tags;
	Node** stack;

	int layers;
	int paths;

	void RestoreEdgeCounts();
	uint32_t FindFlows();
	void GenerateNodeDistances();
	void ResetTags();
	void CreateTags();
};