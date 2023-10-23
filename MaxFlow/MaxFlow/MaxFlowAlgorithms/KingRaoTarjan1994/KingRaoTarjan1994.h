

#pragma once


#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/LinkedList.h"
#include "../../Utilities/LevelLinkedLists.h"
#include <assert.h>
#include "../../Utilities/DynamicTrees/Forest.h"

class KingRaoTarjan1994 : public IMaxFlowAlgorithm
{
private:
	
	struct GameSourceNode;
	struct GameTargetNode;
	struct GameEdge
	{
		Edge* edge;
		GameSourceNode* source;
		GameTargetNode* target;
		LinkedList<GameEdge*>::Node* sourceElement;
		LinkedList<GameEdge*>::Node* targetElement;
	};

	struct GameSourceNode
	{
		GameEdge* DesignatedEdge;
		LinkedList<GameEdge*> InvalidNeighbors;
		LinkedList<GameEdge*>* Neighbors;
		uint32_t removedEdges;
		uint32_t workingEdges;
		GameEdge** edgeMap;
		bool isInU;
	};
	
	struct GameTargetNode
	{
		uint32_t RatioLevel;
		uint32_t EstimatedRatioLevel;
		LinkedList<GameEdge*> Neighbors;
		uint32_t DesignatedEdgeCount;
	};
	
	struct KRTNodeTag
	{
		uint32_t level;
		uint32_t excess;
		uint32_t visibleExcess;
		uint32_t hiddenCapacities;
		uint32_t activeEdges;
		LinkedList<Node*>::Node* linkedListNode;

		void UpdateVisibleExcess()
		{
			if (excess < hiddenCapacities)
				visibleExcess = 0;
			else
				visibleExcess = excess - hiddenCapacities;
		}
	};

	struct ForestNode
	{
		Edge* parentEdge;
	};


	float r0;
	uint32_t l;
	float x;
	uint32_t t;
	uint32_t U;
	uint32_t beta;
	Graph* graph;
	Node* source;
	Node* target;
	Edge** edgesNotYetAdded;
	uint32_t edgesNotYetAddedCount;
	LevelLinkedLists* levelLinkedLists;
	GameSourceNode** gameSourceNodes;
	GameTargetNode** gameTargetNodes;
	LinkedList<Node*> nodeQueue;
	uint32_t flow;
	GameEdge* allGameEdges;

	//e(v) := flow balance in v
	//e*(v) := max(e(v) - total capacity not yet added to E* for edges going into v, 0)


	void Initialize();
	void InitializeEdgeList();
	void InitializeNodeTags();
	void AddSourceEdges();
	void InitializeGameBoard();
	GameSourceNode* GetSourceNode(Node* node);
	GameSourceNode* GetSourceNode(Node* node, uint32_t level);
	GameTargetNode* GetTargetNode(Node* node);
	GameTargetNode* GetTargetNode(Node* node, uint32_t level);
	Node* GetGraphNode(GameTargetNode* node);
	Node* GetGraphNode(GameSourceNode* node);
	int GetK(GameTargetNode* node);
	int GetK(GameSourceNode* node);

	void EdgePush(Edge* edge, uint32_t flow);
	Edge* CurrentEdge(Node* node);
	void Relable(Node* node);
	void Push(Node* node);
	bool IsActive(Edge* edge);
	void SetActive(Edge* edge);
	uint32_t GetId(Node* node);

	void GameSaturatedPush(Edge* edge);
	void GameRelabelingNode(Node* node);
	void GameRemoveNode(GameTargetNode* node);
	void GameRemoveEdge(GameEdge* edge);
	void GameUpdateRatioLevel(GameTargetNode* node);
	void GameUpdateEstimatedRatioLevel(GameTargetNode* node);
	void GameDesignateEdge(GameSourceNode* node);
	void GameForceUpdate(Node* node);


	void Update(GameSourceNode* node);

	void CleanUp();

	//void DebugCheck();
	//void AssertGameConsistency();
public:
	KingRaoTarjan1994(float r0, uint32_t l, float x, uint32_t beta)
	{
		SetParameters(r0, l, x, beta);
	}

	void SetParameters(float r0, uint32_t l, float x, uint32_t beta)
	{
		assert(x >= 2);
		assert(r0*l/x >= 176);
		assert(beta > 0);
		this->r0 = r0;
		this->l = l;
		this->x = x;
		this->beta = beta;
	}

	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);
	//Initialize();
	//uint32_t delta = 2^log2(U)
	//while (delta > 0)														O(logU)
	//	Add all edges with UndirectedCapacity >= delta/beta					O(m)
	//	CreateGame()														O(n^2)
	//	Prepare egiblenodes lists											O(n)
	//	while (|egiblenodes| > 0)
	//		n = egible node with smallest level
	//		if (n.CurrentEdge == null)
	//			relable(n)
	//			Update egiblenodes (n will need to be moved or removed)
	//		else
	//			push(n.currentEdge, min(delta, n.currentEdge.residualCapacity))
	//			Update egiblenodes (n.currentEdge.target might need to be added)
	//	delta /= 2;


	//Initialize()
	//set level of all nodes to 0
	//set level of s to n
	//E* = edges indricent to s

	//Push(edge, flow)
	//e.flow += flow
	//e.rev.flow -= flow;
	//if (e.flow == e.cap)
	//    Game.RemoveEdge(edge)

	//Relable(node)
	//Game.RemoveNode(node)
	//node.label++;


	//--------------GAME----------------

	//Uk := nodes u with degree >= l indicent to nodes v with ratio level >= k

	//CreateGame()
	//...

	//RemoveNode(node)
	//For each edge from node
	//	RemoveEdge(edge)

	//RemoveEdge(edge)
	//u = GetNode(edge.from)
	//v = GetNode(edge.to)
	//Remove v from u's neighbor list
	//if edge is the designated edge
	//	if the degree of u falls below l 
	//		UpdateRatioLevel(v)
	//		if (v.ratioLevel < v.estimatedRatioLevel - 1)
	//			UpdateEstimatedRatioLevel(v)
	//	DesignateEdge(u)
	//	if (u.DesignatedEdge != null && u.DesignatedEdge.ratioLevel == t)
	//		while (any node v' satisfies v.ratioLevel == t)
	//			Reset()

	//UpdateRatioLevel(v)
	//...

	//UpdateEstimatedRatioLevel(v)
	//v.estimatedRatioLevel = v.ratioLevel
	//For all (u, v) where u.degree >= l
	//	update u's neighbor lists

	//GetNode(node)
	//return nodes[node][node.label]

	//DesignateEdge(u)
	//if (u.degree < l)
	//	u.DesignatedEdge = any egible edge or null
	//else
	//	Find edge (u, v) such that e.estimatedRatioLevel is minimal
	//		u.DesignatedEdge = (u, v)
	//		UpdateRatioLevel(v)
	//		if (v.ratioLevel < v.estimatedRatioLevel - 1)
	//			UpdateEstimatedRatioLevel(v)

	//Reset()
	//k = t
	//while |Uk-3| >= r[k-3]*l*|Uk|/(88x)
	//	k -= 3
	//W[k-1] = U[k-1]
	//For each v in V[k-1]
	//	while v.ratioLevel >= k - 1
	//		Undesignate any edge to v
	//		UpdateRatioLevel(v)
	//	if v.ratioLevel > v.estimatedRatioLevel || v.ratioLevel < v.estimatedRatioLevel - 1
	//		UpdateEstimatedRatioLevel(v)
	//	For each u in W[k-1] without a designated edge
	//		DesignateEdge(u)


	//Datastructure
	//Linked list of all edges, remove from it as edges are added to the graph
	//Array of linked lists corrosponding to egible nodes according to level
	//2D array of KRTSourceNode with LinkedLists of neighbors according to ratiolevel, and a designated edge
	//2D array of KRTTargetNode with estimatedRatioLevel, and ratioLevel
};