#include "EdmundKarp.h"
#include <iostream>

#include <algorithm>
#include <queue>
#include <stdlib.h>
#include <array>

using namespace std;

uint32_t EdmundKarp::CalculateMaxFlow(Graph* g, Node* s, Node* t)
{
	//paths = 0;
	ownQueue = new Queue<Node*>(g->NodeCount);
	//stdQueue = new deque<Node*>; 
	bfsTags = new BFSTag[g->NodeCount];
	for(uint32_t i = 0; i < g->NodeCount; i++)
	{
		bfsTags[i].backEdge = NULL;
		bfsTags[i].BFSDepth= UINT32_MAX;
		bfsTags[i].iterationProcessed = 0;
		g->Nodes[i].Tag = &bfsTags[i];
	}
	uint32_t maxFlow = 0;
	uint32_t iteration = 1;

	((BFSTag*)s->Tag)->BFSDepth=0;
	while(FindAugmentingPathBFS(g,s,t,iteration))
	{
		int change = MinCapacityOnPath(s,t);
		UpdatePath(s,t,change);
		maxFlow += change;
		++iteration;
		//paths++;
	}
	//cout << "    [Found " << paths << " augmenting paths]    ";
	return maxFlow;
}

bool EdmundKarp::FindAugmentingPathBFS(Graph* g, Node* s, Node* t, uint32_t iteration)
{
	ownQueue->Clear();
	//stdQueue->clear();
	
	ownQueue->Enqueue(s);
	//stdQueue->push_back(s);
	((BFSTag*)s->Tag)->iterationProcessed = iteration;
	//while(ownQueue->GetSize()>0)
	//while(!stdQueue->empty())
	while(true)
	{
		Node* currentNode = ownQueue->Dequeue();
		if(currentNode == NULL)
			return false;
		//Node* currentNode = stdQueue->front();
		//stdQueue->pop_front();
		for(uint32_t i = 0; i< currentNode->EdgeCount; i++)		//Add all nodes reachable from s
		{
			Edge* newEdge = &(currentNode->Edges)[i];
			if(newEdge->Capasity !=0 )
			{
				Node* targetNode = newEdge->TargetNode;
				BFSTag* tag = ((BFSTag*)targetNode->Tag);
				if(tag->iterationProcessed != iteration)
				{
					tag->BFSDepth = ((BFSTag*)currentNode->Tag)->BFSDepth+1;
					tag->backEdge = newEdge;
					//cout << "Cap: " << tag->backEdge->Capasity << endl;
					tag->iterationProcessed=iteration;
					if(targetNode == t)
						return true;
					ownQueue->Enqueue(newEdge->TargetNode);
					//stdQueue->push_back(newEdge->TargetNode);
				}
			}
		}
	}
}
uint32_t EdmundKarp::MinCapacityOnPath(Node* s, Node* t)
{
	Node* currentNode = t;
	uint32_t min = UINT32_MAX;
	while(currentNode!=s)
	{
		Edge* followEdge = ((BFSTag*)currentNode->Tag)->backEdge;
		if(followEdge->Capasity < min)
			min = followEdge->Capasity;
		//min = std::min(min, followEdge->Capasity);
		//Edge* nextEdge = followEdge->LinkedEdge;
		currentNode = followEdge->LinkedEdge->TargetNode;
	}
	return min;
}

void EdmundKarp::UpdatePath(Node* s, Node* t, uint32_t change)
{
	Node* currentNode = t;
	while(currentNode!=s)
	{
		Edge* followEdge = ((BFSTag*)currentNode->Tag)->backEdge;
		followEdge->Capasity-=change;
		followEdge->LinkedEdge->Capasity +=change;
		currentNode = followEdge->LinkedEdge->TargetNode;
	}
}



	