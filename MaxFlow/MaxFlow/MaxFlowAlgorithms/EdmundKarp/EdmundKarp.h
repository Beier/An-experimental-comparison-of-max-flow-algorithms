#pragma once
#include "../../Graph/Graph.h"
#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/Queue.h"
#include <queue>

struct BFSTag
{
	Edge* backEdge;
	uint32_t BFSDepth;
	uint32_t iterationProcessed;
};

class EdmundKarp : public IMaxFlowAlgorithm
{
public:
	uint32_t paths;
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);
	virtual size_t GetByteSize(Graph* g)
	{
		ownQueue = new Queue<Node*>(g->NodeCount);
		size_t ret = g->GetByteSize() + sizeof(EdmundKarp) + g->NodeCount*sizeof(BFSTag) + ownQueue->GetByteSize();
		delete ownQueue;
		return ret;
	}
private:
	bool FindAugmentingPathBFS(Graph* g, Node* s, Node* t, uint32_t iteration);
	uint32_t MinCapacityOnPath(Node* s, Node* t);
	void UpdatePath(Node* s, Node* t, uint32_t change);
	BFSTag* bfsTags;
	Queue<Node*>* ownQueue;
	std::deque<Node*>* stdQueue;
};

