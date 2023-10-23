#include "Graph.h"



EdgeEnumerator::EdgeEnumerator(Graph* graph)
{
	this->graph = graph;
	Reset();
}

void EdgeEnumerator::Reset()
{
	currentNode = 0;
	currentEdge = 0;
	nextCount = 0;
}

bool EdgeEnumerator::HasNextEdge()
{
	return currentNode != 0 && currentEdge - currentNode->Edges < currentNode->EdgeCount - 1;
}

bool EdgeEnumerator::HasNextNode()
{
	return currentNode == 0 && graph->NodeCount > 0 || currentNode != 0 && currentNode - graph->Nodes < graph->NodeCount - 1;
}

bool EdgeEnumerator::HasNext()
{	
	Node* oldNode = currentNode;
	Edge* oldEdge = currentEdge;

	bool result = Next();
	
	currentNode = oldNode;
	currentEdge = oldEdge;
	return result;
}

bool EdgeEnumerator::Next()
{
	nextCount++;
	if (HasNextEdge())
	{
		currentEdge++;
		return true;
	}
	while (HasNextNode())
	{
		if (currentNode == 0)
			currentNode = graph->Nodes;
		else
			currentNode++;
		currentEdge = currentNode->Edges;
		if (currentNode->EdgeCount > 0)
			return true;
	}
	return false;
}

Edge* EdgeEnumerator::GetCurrent()
{
	return currentEdge;
}