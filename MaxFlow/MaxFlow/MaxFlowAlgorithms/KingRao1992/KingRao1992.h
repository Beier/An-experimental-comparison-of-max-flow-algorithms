#pragma once

#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/LinkedList.h"
#include "../../Utilities/OwnDynamicTree/DynamicTree.h"
#include "../../Utilities//Queue.h"




class KR1992 : public IMaxFlowAlgorithm
{
protected:
	class Game;
	class ForestNode
	{
	};
	class NodeTag
	{
	public:
		uint32_t label;
		uint32_t visibleExcess;
		uint32_t hiddenCapacities;
		uint32_t excess;
		uint32_t globalRelabelIteration;
	};

	Graph* graph;
	Node* source;
	Node* target;
	Game* game;
	DynamicForest<Node>* forest;
	Edge** edgesToAdd;
	uint32_t edgesToAddLength;
	LinkedList<Node*> activeNodes;


	NodeTag* nodeTags;


	virtual void Initialize();
	static int edgepComparator(const void * a, const void * b);
	void TreePush(Node* node);
	virtual void Relable(Node* node);
	void AddEdge(Edge* edge);
	void Saturate(Edge* edge);
	void UpdateVisibleExcess(Node* node);
	virtual void CleanUp();
	
	int graphNr;
	uint32_t GetCapacity(Edge* edge);

	
	virtual void FinishProcessingNode(Node* node) {}
public:

	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target);



protected:
	class Game
	{
	private:
		class SourceNode;
		class TargetNode;
		class GameEdge
		{
		public:
			TargetNode* target;
			SourceNode* source;
			Edge* edge;

			LinkedList<GameEdge*>::Node* edgeListTag;
			LinkedList<GameEdge*>::Node* edgeByErlTag;
			LinkedList<GameEdge*>::Node* targetTag;
		};

		class SourceNode
		{
		public:
			uint32_t degree;
			GameEdge* designatedEdge;

			LinkedList<GameEdge*> edges;
			LinkedList<GameEdge*>* edgesByErl;
			GameEdge** edgeLookup;
		};

		class TargetNode
		{
		public:
			uint32_t designatedDegree;
			uint32_t erl;
			uint32_t rl;
			LinkedList<GameEdge*> edges;

		};
		
		float r0;
		uint32_t l;
		uint32_t t;

		Graph* graph;
		DynamicForest<Node>* forest;
		KR1992* alg;

		SourceNode* sourceNodes;
		TargetNode* targetNodes;
		GameEdge* edges;
		bool resetting;

		SourceNode* GetSource(Node* node, uint32_t level)
		{
			return sourceNodes + (node - graph->Nodes) + level*graph->NodeCount;
		}

		TargetNode* GetTarget(Node* node, uint32_t level)
		{
			return targetNodes + (node - graph->Nodes) + level*graph->NodeCount;
		}

		Node* GetNode(TargetNode* node)
		{
			return graph->Nodes + (node - targetNodes)%graph->NodeCount;
		}

		Node* GetNode(SourceNode* node)
		{
			return graph->Nodes + (node - sourceNodes)%graph->NodeCount;
		}

		uint32_t GetLevel(SourceNode* node)
		{
			return (node - sourceNodes)/graph->NodeCount;
		}

		void DesignateEdge(SourceNode* node);
		void UpdateRl(TargetNode* node);
		void UpdateErl(TargetNode* node);
		void Reset();
		void UpdateForest(SourceNode* node);
		void UpdateForest(Node* node);
		void UpdateForest(Node* node, uint32_t level);

		void Cut(SourceNode* node);
		void Link(SourceNode* node);
		


	public:
		void Cut(Node* node, uint32_t level);
		void Link(Node* node, uint32_t level);
		Game(KR1992* alg);
		~Game();
		void EdgeKill(Edge* edge, uint32_t label);
		void NodeKill(Node* node, uint32_t label);
		Edge* CurrentEdge(Node* node, uint32_t label);

	};
};

class KR1992GlobalRename : public KR1992
{
protected:
	bool performingGlobalRelabel;
	Queue<Node*>* globalRelabelQueue;
	
	
	virtual void Initialize() override;
	virtual void CleanUp() override;
	virtual void Relable(Node* node) override;
	virtual void GlobalRelabel();
};



class KR1992RareGlobalRename : public KR1992GlobalRename
{
	int nodesProcessed;
	Node* passEndNode;
	uint32_t excessLastRelabel;
	uint32_t excessLastPass;
	virtual void Initialize() override;
	virtual void Relable(Node* node) override;
	virtual void GlobalRelabel() override;
	virtual void FinishProcessingNode(Node* node) override;
};

