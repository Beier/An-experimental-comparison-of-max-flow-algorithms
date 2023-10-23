#pragma once
#include "../../Graph/Graph.h"
#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/Queue.h"
#include <queue>
#include "../../Utilities/OwnDynamicTree/DynamicTree.h"
#include <iostream>

class GoldbergTarjanDyn : public IMaxFlowAlgorithm
{
protected:
	struct GTTag
	{
		Edge* currentEdge;
		uint32_t excess;
		uint32_t depth;
		Edge* pushEdge;
		uint32_t globalRelabelIteration;
	};

public:
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);
	virtual uint32_t GetByteSize();
protected:
	Queue<Node*>* m_queue;
	Graph* m_graph;
	DynamicForest<Node>* m_forest;
	uint32_t m_maxSize;
	uint32_t imageNr;
	Node* source;
	Node* target;
	GTTag* gttags;

	virtual void Initialize();
	virtual void CleanUp() override;
	void TreePushRelabel(Node* v);
	void Push(Edge* e);
	void Send(Node* n);
	virtual void Relabel(Node* n);
	void Discharge(Node* source, Node* target);
	void TransferChangesFromDynamicTree();

	void PrintGraph(Node* source, Node* target, Node* currentNode);

	bool AssertExcessHolds(Node* n, uint32_t inGoingCap, uint32_t outGoingCap);
	void calcCap(Node* n, uint32_t& inGoingCap, uint32_t& outGoingCap);

	bool AssertCorrectPushEdge(Node* n);
	virtual void FinishProcessingNode(Node* node) {}

};

class GoldbergTarjanDynGlobalRename : public GoldbergTarjanDyn
{
protected:
	bool performingGlobalRelabel;
	Queue<Node*>* globalRelabelQueue;
	
	
	virtual void Initialize() override;
	virtual void CleanUp() override;
	virtual void Relabel(Node* node) override;
	void RelabelTo(Node* node, uint32_t targetLabel);
	virtual void GlobalRelabel();
	uint32_t GetCapacity(Edge* edge);
	virtual uint32_t GetByteSize() override;
};


class GoldbergTarjanDynRareGlobalRename : public GoldbergTarjanDynGlobalRename
{
protected:
	Node* passEndNode;
	uint32_t excessLastRelabel;
	uint32_t excessLastPass;
	virtual void Initialize() override;
	virtual void Relabel(Node* node) override;
	virtual void GlobalRelabel() override;
	virtual void FinishProcessingNode(Node* node) override;
	virtual uint32_t GetByteSize() override;
};

class GoldbergTarjanDynRareGlobalRename2 : public GoldbergTarjanDynRareGlobalRename
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