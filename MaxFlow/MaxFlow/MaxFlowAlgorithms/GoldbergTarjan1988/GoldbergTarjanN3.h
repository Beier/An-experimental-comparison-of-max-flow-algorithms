#pragma once
#include "../../Graph/Graph.h"
#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/Queue.h"
#include <queue>

struct GTTag
{
	Edge* currentEdge;
	uint32_t excess;
	uint32_t depth;
	uint32_t globalRelabelIteration;
};

class GoldbergTarjanN3 : public IMaxFlowAlgorithm
{
public:
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);	//Discharge in the article
protected:
	Queue<Node*>* m_queue;
	Graph* m_graph;
	GTTag* gttags;
	Node* source;
	Node* target;

	virtual void Initialize();
	virtual void CleanUp() override;
	void PushRelabel(Node* v);
	void Push(Edge* e);
	virtual void Relabel(Node* n);
	void SwapIfZeroCap(Edge** e);
	void Discharge(Graph* graph, Node* source, Node* target);
	uint32_t moveZeroEdgesToEnd(Node* n);

	//void PrintGraph(Node* source, Node* target, Node* currentNode);
	uint32_t imageNr;

	virtual void FinishProcessingNode(Node* node){}
	virtual uint32_t GetByteSize()
	{
		return sizeof(GoldbergTarjanN3) + m_graph->GetByteSize() + m_graph->NodeCount*sizeof(GTTag) + m_queue->GetByteSize();
	}
};


class GoldbergTarjanN3GlobalRename : public GoldbergTarjanN3
{
protected:
	bool performingGlobalRelabel;
	Queue<Node*>* globalRelabelQueue;
	
	
	virtual void Initialize() override;
	virtual void CleanUp() override;
	virtual void Relabel(Node* node) override;
	virtual void GlobalRelabel();
	void RelabelTo(Node* node, uint32_t targetLabel);
};

class GoldbergTarjanN3RareGlobalRename : public GoldbergTarjanN3GlobalRename
{
protected:
	Node* passEndNode;
	uint32_t excessLastRelabel;
	uint32_t excessLastPass;
	virtual void Initialize() override;
	virtual virtual virtual void Relabel(Node* node) override;
	virtual void GlobalRelabel() override;
	virtual void FinishProcessingNode(Node* node) override;
};

class GoldbergTarjanN3RareGlobalRename2 : public GoldbergTarjanN3RareGlobalRename
{
public:
	virtual void SetCheckLimit(uint32_t limit) override;
protected:
	virtual void Initialize() override;
	uint32_t nodesProcessed;
	uint32_t nodesProcessLimit;
	virtual void FinishProcessingNode(Node* node) override;
	virtual void GlobalRelabel() override;
};