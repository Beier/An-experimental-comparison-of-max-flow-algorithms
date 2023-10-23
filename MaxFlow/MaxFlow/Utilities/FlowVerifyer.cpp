#include "FlowVerifyer.h"
#include <iostream>
#include "Queue.h"

using namespace std;



Edge* GetCorrospondingEdge(Graph* g1, Graph* g2, Node* g2n, Edge* g1e)
{
	int targetNode = g1e->TargetNode - g1->Nodes;

	Edge* candidate = g2n->Edges + (g1e - g1e->LinkedEdge->TargetNode->Edges);
	if (candidate->TargetNode -g2->Nodes == targetNode)
		return candidate;

	for (int i = 0; i < g2n->EdgeCount; i++)
	{
		if (g2n->Edges[i].TargetNode - g2->Nodes == targetNode)
		{
			return g2n->Edges + i;
		}
	}
	return NULL;
}

bool DoGraphsCorrospond(Graph* originalGraph, Graph* residualGraph)
{
	if (originalGraph->NodeCount != residualGraph->NodeCount)
		return false;
	for (int n = 0; n < originalGraph->NodeCount; n++)
	{
		Node* originalNode = originalGraph->Nodes + n;
		Node* residualNode = residualGraph->Nodes + n;
		if (originalNode->EdgeCount != residualNode->EdgeCount)
			return false;
		for (int e = 0; e < originalNode->EdgeCount; e++)
		{
			Edge* originalEdge = originalNode->Edges + e;
			Edge* residualEdge = GetCorrospondingEdge(originalGraph, residualGraph, residualNode, originalNode->Edges + e);
			if (residualEdge == NULL)
				return false;
			if (originalEdge->Capasity + originalEdge->LinkedEdge->Capasity != residualEdge->Capasity + residualEdge->LinkedEdge->Capasity)
				return false;
		}
	}
	return true;
}

Graph* GenerateFlowGraph(Graph* originalGraph, Graph* residualGraph)
{
	originalGraph->SortEdges();
	residualGraph->SortEdges();
	if (!DoGraphsCorrospond(originalGraph, residualGraph))
		return NULL;
	Graph* flowGraph = new Graph();
	flowGraph->NodeCount = originalGraph->NodeCount;
	flowGraph->EdgeCount = originalGraph->EdgeCount;
	flowGraph->Nodes = new Node[flowGraph->NodeCount];
	for (int n = 0; n < originalGraph->NodeCount; n++)
	{
		Node* originalNode = originalGraph->Nodes + n;
		Node* flowNode = flowGraph->Nodes + n;
		flowNode->EdgeCount = originalNode->EdgeCount;
		flowNode->Edges = new Edge[flowNode->EdgeCount];
	}
	for (int n = 0; n < originalGraph->NodeCount; n++)
	{
		Node* originalNode = originalGraph->Nodes + n;
		Node* residualNode = residualGraph->Nodes + n;
		Node* flowNode = flowGraph->Nodes + n;
		for (int e = 0; e < originalNode->EdgeCount; e++)
		{
			Edge* originalEdge = originalNode->Edges + e;
			Edge* residualEdge = GetCorrospondingEdge(originalGraph, residualGraph, residualNode, originalNode->Edges + e);
			Edge* flowEdge = flowNode->Edges + e;
			if(originalEdge->Capasity  > residualEdge->Capasity)
				flowEdge->Capasity = originalEdge->Capasity - residualEdge->Capasity;
			else
				flowEdge->Capasity = 0;
			flowEdge->TargetNode = flowGraph->Nodes + (originalEdge->TargetNode - originalGraph->Nodes);
			flowEdge->LinkedEdge = flowEdge->TargetNode->Edges + (originalEdge->LinkedEdge - originalEdge->TargetNode->Edges);
		}
	}
	return flowGraph;
}

uint32_t GetOutgoingFlow(Node* node)
{
	uint32_t flow = 0;
	for (int e = 0; e < node->EdgeCount; e++)
		flow += node->Edges[e].Capasity;
	return flow;
}

uint32_t GetIncomingFlow(Node* node)
{
	uint32_t flow = 0;
	for (int e = 0; e < node->EdgeCount; e++)
		flow += node->Edges[e].LinkedEdge->Capasity;
	return flow;
}


int64_t GetFlow(Node* node)
{
	int64_t flow = 0;
	for (int e = 0; e < node->EdgeCount; e++)
		flow += (int64_t)node->Edges[e].Capasity - (int64_t)node->Edges[e].LinkedEdge->Capasity;
	return flow;
}

bool IsValidFlow(Graph* originalGraph, Graph* residualGraph, uint32_t maxFlow)
{
	Graph* flowGraph = GenerateFlowGraph(originalGraph, residualGraph);
	if (flowGraph == NULL)
	{
		cout << "Flowgraph is null" << endl;
		return false;
	}
	//FlowOut(s) = FlowIn(t) = maxFlow
	if (GetFlow(flowGraph->Nodes) != maxFlow || -GetFlow(flowGraph->Nodes + flowGraph->NodeCount - 1) != maxFlow)
	{
		cout << "Flow out of source or flow into sink is not equal to maxflow " << endl;
		return false;
	}

	
	//FlowOut(v) = FlowIn(v) for v != s && v != t
	for (int n = 1; n < flowGraph->NodeCount - 1; n++)
		if (GetFlow(flowGraph->Nodes + n) != 0)
		{
			cout << "Flow Conservation constraint violated: " << GetFlow(flowGraph->Nodes + n) << endl;
			return false;
		}

	//0 <= Flow(e) <= Capasity(e)
	for (int n = 0; n < flowGraph->NodeCount; n++)
	{
		Node* flowNode = flowGraph->Nodes + n;
		Node* originalNode = originalGraph->Nodes + n;
		for (int e = 0; e < originalNode->EdgeCount; e++)
		{
			Edge* flowEdge = flowNode->Edges + e;
			Edge* originalEdge = GetCorrospondingEdge(flowGraph, originalGraph, originalNode, flowEdge);
			if (originalEdge == NULL)
			{
				cout << "Couldn't find the edge in the original graph corresponding to flow-edge"<< endl;
				return false;
			}
			if (flowEdge->Capasity < 0 || originalEdge->Capasity < flowEdge->Capasity)
			{
				cout << "Either negative flow or the flow breaks the capacity constraint" << endl;
				return false;
			}
		}
	}

	//No augmenting path
	for (int n = 0; n < residualGraph->NodeCount; n++)
	{
		Node* node = residualGraph->Nodes + n;
		node->Tag = false;
	}
	Queue<Node*> queue(residualGraph->NodeCount);
	queue.Enqueue(residualGraph->Nodes);
	residualGraph->Nodes->Tag = (void*)true;
	while (queue.GetSize() > 0)
	{
		Node* node = queue.Dequeue();
		if (node == residualGraph->Nodes + residualGraph->NodeCount - 1)
		{
				cout << "Found augmenting path - this is not a max flow." << endl;
				return false;
		}
		for (int e = 0; e < node->EdgeCount; e++)
		{
			Edge* edge = node->Edges + e;
			if (edge->Capasity == 0)
				continue;
			Node* target = edge->TargetNode;
			if (!target->GetTag<bool>())
			{
				target->Tag = (void*)true;
				queue.Enqueue(target);
			}
		}

	}

	delete flowGraph;
	return true;
}