#pragma once

#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/LinkedList.h"
#include "../../Utilities/Queue.h"
#include "../../Utilities/OwnDynamicTree/DynamicTree.h"

#include <string>    
#include <sstream>  


class KrtModDyn : public IMaxFlowAlgorithm
{
protected:
	struct EdgeTag
	{
		LinkedList<EdgeTag*>::Node* sourceElm;
		LinkedList<EdgeTag*>::Node* sourceErlElm;
		LinkedList<EdgeTag*>::Node* targetElm;
		Edge* edge;
		bool isAdded;
	};

	struct SourceNode
	{
		EdgeTag* currentEdge;
		LinkedList<EdgeTag*> activeEdges;
		LinkedList<EdgeTag*>* activeEdgesByErl;
		LinkedList<EdgeTag*> levelInegibleEdges;
	};

	struct TargetNode
	{
		uint32_t designatedEdges;
		uint32_t rl;
		uint32_t erl;
		LinkedList<EdgeTag*> activeEdges;
	};

	struct NodeTag
	{
		SourceNode source;
		TargetNode target;
		uint32_t hiddenCapacities;
		uint32_t excess;
		uint32_t visibleExcess;
		uint32_t lable;
		uint32_t globalRelabelIteration;
	};

	
	DynamicForest<Node>* forest;
	
	Graph* graph;
	Node* source;
	Node* target;
	uint32_t t;
	uint32_t l;
	float r0;
	Edge** edgesToInsert;
	LinkedList<Node*> readyNodes;
	bool resetting;
	NodeTag* nodeTags;
	EdgeTag* edgeTags;
	virtual void FinishProcessingNode(Node* node) {}
public:


	virtual void Initialize();
	void AddEdge(Edge* edge);
	void Saturate(Edge* edge);
	void TreePush(Node* node);
	virtual void Relable(Node* node);
	Edge* CurrentEdge(Node* node);
	void UpdateVisibleExcess(Node* node);
	void DesignateEdge(Node* node);
	void UpdateErl(Node* node);
	void UpdateRl(Node* node);
	void Reset();
	virtual void CleanUp() override;

	void Cut(Node* node);
	void Link(Node* node);

	uint32_t GetCapacity(Edge* edge);




	static int edgeComparator(const void * a, const void * b);
	static int edgepComparator(const void * a, const void * b);

	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);
	/*
	int graphNr;
	void PrintGraph(Node* activeNode, Edge* activeEdge);*/
	//void AssertConsistent();
};

class KrtModDynGlobalRename : public KrtModDyn
{
protected:
	bool performingGlobalRelabel;
	Queue<Node*>* globalRelabelQueue;
	
	
	virtual void Initialize() override;
	virtual void CleanUp() override;
	virtual void Relable(Node* node) override;
	void RelableTo(Node* node, uint32_t label);
	virtual void GlobalRelabel();
};

class KrtModDynRareGlobalRename : public KrtModDynGlobalRename
{
protected:
	Node* passEndNode;
	uint32_t excessLastRelabel;
	uint32_t excessLastPass;
	virtual void Initialize() override;
	virtual void Relable(Node* node) override;
	virtual void GlobalRelabel() override;
	virtual void FinishProcessingNode(Node* node) override;
};

class KrtModDynRareGlobalRename2 : public KrtModDynRareGlobalRename
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
