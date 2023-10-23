
#pragma once

#include "../MaxFlowAlgorithm.h"
#include "../../Utilities/Queue.h"

template <class T>
class ParallelAlgorithm : IMaxFlowAlgorithm
{
	struct NodeTag
	{
		uint32_t SourceDist;
		uint32_t TargetDist;
	};


	void CalculateDistances(Graph* graph, Node* source, Node* target)
	{
		NodeTag* tag;
		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
		{
			tag = node->GetTag<NodeTag*>();
			tag->SourceDist = graph->NodeCount;
			tag->TargetDist = graph->NodeCount;
		}

		Queue<Node*>* nodeQueue = new Queue<Node*>(graph->NodeCount);
		//Source
		nodeQueue->Enqueue(source);
		tag = source->GetTag<NodeTag*>();
		tag->SourceDist = 0;
		while (!nodeQueue->IsEmpty())
		{
			Node* node = nodeQueue->Dequeue();
			tag = node->GetTag<NodeTag*>();
			uint32_t dist = tag->SourceDist + 1;
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->Capasity == 0)
					continue;
				Node* target = edge->TargetNode;
				tag = target->GetTag<NodeTag*>();
				if (dist < tag->SourceDist)
				{
					tag->SourceDist = dist;
					nodeQueue->Enqueue(target);
				}
			}
		}

		//Target
		nodeQueue->Enqueue(target);
		tag = target->GetTag<NodeTag*>();
		tag->TargetDist = 0;
		while (!nodeQueue->IsEmpty())
		{
			Node* node = nodeQueue->Dequeue();
			tag = node->GetTag<NodeTag*>();
			uint32_t dist = tag->TargetDist + 1;
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->LinkedEdge->Capasity == 0)
					continue;
				Node* target = edge->TargetNode;
				tag = target->GetTag<NodeTag*>();
				if (dist < tag->TargetDist)
				{
					tag->TargetDist = dist;
					nodeQueue->Enqueue(target);
				}
			}
		}


		delete nodeQueue;
	}

	void Partition(Graph* graph)
	{
		
		Graph* leftGraph = new Graph();
		leftGraph->NodeCount = leftGraph->NodeCount/2 + 1;
		if (graph->NodeCount & 1)
			leftGraph->NodeCount++;
		leftGraph->Nodes = new Node[rightGraph->NodeCount];

		Graph* rightGraph = new Graph();
		rightGraph->NodeCount = leftGraph->NodeCount/2 + 1;
		rightGraph->Nodes = new Node[rightGraph->NodeCount];

		Node* leftNode = leftGraph->Nodes;
		Node* rightNode = rightGraph->Nodes + 1;
		

		bool tieToLeft = false;
		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
		{
			NodeTag* tag = node->GetTag<NodeTag*>();

			if (tag->SourceDist == tag->TargetDist)
				tieToLeft = !tieToLeft;

			Node* target;
			if (tag->SourceDist < tag->TargetDist || tag->SourceDist == tag->TargetDist && tieToLeft)
				target = leftNode++;
			else
				target = rightNode++;
			*target = *node;
			node->Tag = target;
		}


		rightNode = rightGraph->Nodes;

		leftNode->EdgeCount = 0;
		rightNode->EdgeCount = 0;
		
		for (Node* node = leftNode->Nodes; node < leftNode->Nodes + leftNode->NodeCount - 1; node++)
		{
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				edge->TargetNode = edge->TargetNode->Tag;
				if (edge->TargetNode < leftGraph->Nodes || edge->TargetNode >= leftGraph->Nodes + leftGraph->NodeCount)
				{
					edge->TargetNode = leftNode
				}
			}
		}

	}

	uint32_t CalculateMaxFlowInternal(Graph* graph, Node* source, Node* target, uint32_t depth)
	{
		uint32_t flow;
		int instances = 1 << depth;
		if (instances > 16 || graph->NodeCount <= 32)
		{
			T* alg = new T();
			flow = alg->CalculateMaxFlow(graph, source, target);
			delete alg;
			return flow;
		}

		CalculateDistances(graph, source, target);


	}


public:
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target)
	{
		NodeTag* tags = new NodeTag[graph->NodeCount];
		NodeTag* tag = tags;
		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++, tag++)
			node->Tag = tag;


		uint32_t maxFlow = CalculateMaxFlowInternal(graph, source, target, 0);

		delete[] tags;

		return maxFlow;
	}
};