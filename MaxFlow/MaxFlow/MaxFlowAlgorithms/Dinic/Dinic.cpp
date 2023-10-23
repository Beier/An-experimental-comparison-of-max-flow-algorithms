#include "Dinic.h"





void Dinic::CreateTags()
{
	tags = new DinicTag[graph->NodeCount];

	DinicTag* pTag = tags;
	Node* pNode = graph->Nodes;
	for (uint32_t i = 0; i < graph->NodeCount; i++)
	{
		pTag->Distance = 0;
		pTag->OriginalEdgeCount = pNode->EdgeCount;
		pNode->Tag = pTag;
		pNode++;
		pTag++;
	}
 }

 void Dinic::ResetTags()
 {
	 int length = graph->NodeCount;
	 DinicTag* tag = tags;
	 while (length-- > 0)
		 (tag++)->Distance = 0;
 }
 

 void Dinic::GenerateNodeDistances()
 {
	 ResetTags();
	 queue->Enqueue(source);
	 uint32_t* targetNodeDist = &target->GetTag<DinicTag*>()->Distance;
	 *targetNodeDist = graph->NodeCount;

	 while (queue->GetSize() > 0)
	 {
		 Node* pNode = queue->Dequeue();
		 DinicTag* tag = pNode->GetTag<DinicTag*>();

		 pNode->EdgeCount = 0;
		 if (tag->Distance >= *targetNodeDist)
			 continue;

		 Edge* pEdge = pNode->Edges;
		 Edge* pUnusedEdge = pEdge;
		 for (uint32_t i = 0; i < tag->OriginalEdgeCount; i++)
		 {
			 Node* targetNode = pEdge->TargetNode;
			 DinicTag* targetTag = targetNode->GetTag<DinicTag*>();

			 if (pEdge->Capasity > 0)
			 {
				bool uselessEdge = false;
				if (targetTag->Distance == 0 && targetNode != source || targetNode == target)
				{
					//Unprocessed node
					targetTag->Distance = tag->Distance + 1;
					queue->Enqueue(targetNode);
				}
				else
				{
					uselessEdge = targetTag->Distance != tag->Distance + 1;
				}
				if (!uselessEdge)
				{
					Graph::SwapEdges(pEdge, pUnusedEdge);
					pUnusedEdge++;
					pNode->EdgeCount++;
				}
			 }
			 pEdge++;
		 }
		 
	 }
 }



 uint32_t Dinic::FindFlows()
 {
	 uint32_t totalFlow = 0;
	 
	 for (DinicTag* tag = tags; tag < tags + graph->NodeCount; tag++)
	 {
		 tag->pBack = 0;
		 tag->currentEdge = 0;
	 }
	 
	 Node* node = source;
	 node->GetTag<DinicTag*>()->currentEdge = node->Edges;

	 while (true)
	 {
		 DinicTag* tag = node->GetTag<DinicTag*>();
		 if (node == target)
		 {
			 paths++;
			 //Calculate minCapacity
			 uint32_t minCapacity = tag->pBack->LinkedEdge->Capasity;
			 for (Node* backNode = node; backNode != source; )
			 {
				DinicTag* backTag = backNode->GetTag<DinicTag*>();
				uint32_t capacity = backTag->pBack->LinkedEdge->Capasity;
				if (minCapacity > capacity)
					minCapacity = capacity;
				backNode = backTag->pBack->TargetNode;
			 }
			 //Send flow
			 for (Node* backNode = node; backNode != source; )
			 {
				DinicTag* backTag = backNode->GetTag<DinicTag*>();
				Edge* edge = backTag->pBack->LinkedEdge;
				edge->Capasity -= minCapacity;
				edge->LinkedEdge->Capasity += minCapacity;
				
				backNode = backTag->pBack->TargetNode;
				if (edge->Capasity == 0)
				{
					Graph::SwapEdges(edge, backNode->Edges + backNode->EdgeCount - 1);
					backNode->EdgeCount--;
					//Jump back behind first 0 node
					node = backNode;
				}
			 }
			 totalFlow += minCapacity;
			 continue;
		 }
		 if (tag->currentEdge == node->Edges + node->EdgeCount)
		 {
			 if (node == source)
				 break;
			 node = tag->pBack->TargetNode;
			 tag = node->GetTag<DinicTag*>();
			 if (node->EdgeCount > 0)
			 {
				 Graph::SwapEdges(tag->currentEdge, node->Edges + node->EdgeCount - 1);
				 node->EdgeCount--;
			 }
			 continue;
		 }
		 Edge* backEdge = tag->currentEdge->LinkedEdge;
		 node = tag->currentEdge->TargetNode;
		 tag = node->GetTag<DinicTag*>();
		 tag->currentEdge = node->Edges;
		 tag->pBack = backEdge;
	 }

	 return totalFlow;
 }

 void Dinic::RestoreEdgeCounts()
 {
	 Node* pNode = graph->Nodes;
	 DinicTag* pTag = tags;
	 for (uint32_t i = 0; i < graph->NodeCount; i++)
	 {
		 (pNode++)->EdgeCount = (pTag++)->OriginalEdgeCount;
	 }
 }
 	
#include <string>    
#include <sstream> 
#include <iostream>
uint32_t Dinic::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	this->graph = graph;
	this->source = source;
	this->target = target;
	paths = 0;
	layers = 0;


	CreateTags();
	this->queue = new Queue<Node*>(graph->NodeCount);
	stack = new Node*[graph->NodeCount];
	 int layer = 0;
	uint32_t totalFlow = 0;
	while (true)
	{
		layers++;
		GenerateNodeDistances();


		uint32_t flow = FindFlows();
		if (flow == 0)
			break;
		totalFlow += flow;

	}
	RestoreEdgeCounts();
	
	std::cout << "    [Layers: " << layers << ", Paths: " << paths << "]   ";
	delete[] stack;
	delete[] tags;
	delete queue;
	return totalFlow;
}

