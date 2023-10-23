#pragma once
#include "../../Graph/Graph.h"
#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/Queue.h"
#include <stack>
#include <vector>
#include "BlockingFlow\BlockingFlow.h"

using namespace std;

	class GRSuperNode
	{
	public:
		uint32_t distance;
		Node** m_subNodes;
		uint32_t m_subNodeCount;
		Edge* m_currentEdge;
		Edge* GetNextEdge();
		bool IsLastEdge();
		bool HasPassedLastEdge();
		Edge* ResetToFirstEdge();
		uint32_t excess;
		uint32_t id;
		bool addedToQueue;
		Edge* treeEdge;
		uint32_t currentEdgeAt;
	};

class GoldbergRao1998 : public IMaxFlowAlgorithm
{
public:


	struct GRTag
	{
		uint32_t distance;
		GRSuperNode* superNode;
		uint32_t currentEdge;
	};

	struct GRRoutingTag
	{
		Edge* InTreeParentEdge;
		Node* InTreeChild;
		Node* InTreeSibling;

		Edge* OutTreeParentEdge;
		Node* OutTreeChild;
		Node* OutTreeSibling;

		uint32_t supply;
		uint32_t demand;
		uint32_t descendantDemand;

		bool visitedMoveSupply;
		bool visitedMoveDemand;
	};

	struct GREdgeTag
	{
		uint32_t newFlow;
	};

	struct GRSuperEdgeTag
	{
		Edge** subEdges;
		uint32_t edgeCount;
		~GRSuperEdgeTag()
		{
			delete subEdges;
		}
	};

	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);
	uint32_t* GetDistances();
private:
	void GenerateNodeDistances(Node* source, Node* target);

	void ContractStronglyConnectedComponents(Node* source, Node* target);
	void GetStronglyConnectedComponents();
	void StrongConnect(Node* v, uint32_t* index, uint32_t* lowlink,  uint32_t* lowestUnusedIndex);

	void RunTarjan();
	void TarjanIter(Node* v, uint32_t* lowestUnusedIndex);

	void ResetSCCsEdges();

	void DecreaseFlow(Node* source, Node* target, uint32_t decreaseBy);

	void ExtendFlowToOriginalGraph(Node* source, Node* target); //, uint32_t* translationMatrixOldToNewNode
	void LinkNodes(Node* child, GRRoutingTag* childTag, GRRoutingTag* parentTag, Edge* parentEdge, bool Intree);
	void LinkInTree(Node* v, uint32_t componentId, uint32_t rootNodeIndex);
	void LinkOutTree(Node* v, uint32_t componentId, uint32_t rootNodeIndex);
	uint32_t CalcDescDemands(uint32_t nodeId);
	void MoveSupplyForward(uint32_t nodeId);
	void MoveDemandBackwards(uint32_t nodeId);

	void ResetTags();

	inline bool IsZeroEdge(Edge* e);
	inline bool IsLargeEdge(Edge* e);
	inline bool IsSpecialEdge(Edge* e);


	uint32_t FindMinimumCanonicalCut(Node* source, Node* target);

	bool CheckAllEdgeCapacities();
	Node* CheckFlowConservation(Node* source, Node* target);
	bool CheckNewFlowForNode(Node* node);

	void PrintDecreaseFlowGraph(GRSuperNode*);
	void PrintGraphWithNodes(Node* source, Node* target);
	void PrintSupplyDemandGraph(Node* source, Node* target, const char* fileName);
	void PrintInTree(Node* source, Node* target, uint32_t SccId);
	void PrintOutTree(Node* source, Node* target, uint32_t SccId);

	uint32_t* m_canonicalCutValues;
	Graph* m_graph;
	Graph* m_graphClone;

	uint32_t m_delta;
	uint32_t F;

	GRRoutingTag* m_routingTags;
	Queue<Node*>* m_InTreeQueue;
	Queue<Node*>* m_OutTreeQueue;
	GRSuperNode* m_SuperNodes;
	uint32_t m_nrSuperNodes;
	Node** m_subNodes;
	GRTag* m_nodeTags;
	GREdgeTag* m_edgeTags;
	BlockingFlow* m_blockingFlowCalculator;
	float m_lamda;
	uint32_t* m_translationMatrixOldToNewNode;
	Node** m_recursionStack;

	//For getting the strongly connected components
	stack<uint32_t>* m_theStack;
	vector<vector<uint32_t>>* SCCs;
	bool* m_isOnStack;

	//For outputting graph images
	uint32_t runNr;
	uint32_t m_graphNrDF;
	//uint32_t m_graphNr;

};