#ifndef BLOCKING_FLOW
#define BLOCKING_FLOW

#include "../../../Utilities/DynamicTrees/Forest.h"
#include "../../../Graph/Graph.h"
#include "../../../Utilities/Queue.h"
#include "../../../Utilities/OwnDynamicTree/DynamicTree.h"

class GRSuperNode;

class BlockingFlow
{

	struct NodeBase
	{
		NodeBase()
		{
			edge = NULL;
		}
		Edge* edge;
	};

public:
	uint32_t CalculateBlockingFlow(Graph* graph, GRSuperNode* superNodes, Node* source, Node*  target, uint32_t delta, uint32_t nrStronglyConnectecomponents, uint32_t runNr, bool checkOutput);
	BlockingFlow(uint32_t nrNodes);
	~BlockingFlow();
private:
	int m_maxSize;
	Graph* m_graph;
	DynamicForest<GRSuperNode>* m_forest;

	Queue<GRSuperNode*>* m_queue;
	uint32_t m_maxNodes;
	bool* m_blocked;
	uint32_t m_delta;
	inline bool EdgeLength(Edge* e);
	uint32_t m_graphNrBF;
	uint32_t m_runNrBF;
	GRSuperNode* m_sourceSuperNode;
	GRSuperNode* m_targetSuperNode;

	void Discharge(Node* source, Node* target, GRSuperNode* superNodes, uint32_t nrSccs);
	void TreePushPullBlock(GRSuperNode* v);
	void Push(Edge* e);
	void Pull(Edge* e);
	void Block(GRSuperNode* v);
	void SendPush(GRSuperNode* n);
	void SendPull(GRSuperNode* n);
	bool TestIsBlockingFlow(Node* source, Node* target);
	void ResetTags(uint32_t resetCount);
	void TransferChangesFromDynamicTree(GRSuperNode* superNodes, uint32_t nrSCCs);


	bool CheckFlow(GRSuperNode* n);
	void PrintGraph(Edge* activeEdge);
	bool* nodesDrawed;
};


#endif