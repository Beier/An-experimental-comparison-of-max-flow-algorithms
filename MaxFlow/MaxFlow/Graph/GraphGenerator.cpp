#include "Graph.h"
#include <algorithm>
using namespace std;



Graph* Graph::GenerateGraph(GenerateGraphFunc generateGraph, GenerateCapacitiesFunc capacityGenerator, uint32_t nodeSize)
{
	Graph* graph = new Graph();
	graph->NodeCount = nodeSize;
	graph->Nodes = new Node[nodeSize];
	vector<uint32_t>* edges = new vector<uint32_t>[nodeSize];
	generateGraph(nodeSize, edges);


		
	int* originalEdgeCount = new int[graph->NodeCount];
	for (uint32_t node = 0; node < graph->NodeCount; node++)
	{
		originalEdgeCount[node] = edges[node].size();
	}
		
	graph->EdgeCount = 0;
	for (uint32_t source = 0; source < graph->NodeCount; source++)
	{
		graph->EdgeCount += edges[source].size();
		for (uint32_t e1 = 0; e1 < edges[source].size(); e1++)
		{
			bool foundLinkedEdge = false;
			for (uint32_t e2 = 0; e2 < edges[edges[source].at(e1)].size(); e2++)
			{
				if (source == edges[edges[source].at(e1)].at(e2))
				{
					foundLinkedEdge = true;
					break;
				}
			}
			if (!foundLinkedEdge)
			{
				if (edges[source].at(e1) <= source)
					graph->EdgeCount++;
				edges[edges[source].at(e1)].push_back(source);
			}
		}
	}
		

		
		
	for (uint32_t source = 0; source < graph->NodeCount; source++)
	{
		graph->Nodes[source].EdgeCount = edges[source].size();
		graph->Nodes[source].Edges = new Edge[edges[source].size()];
		Edge* e = graph->Nodes[source].Edges;
		for (uint32_t edgeIndex = 0; edgeIndex < edges[source].size(); edgeIndex++)
		{
			uint32_t target = edges[source].at(edgeIndex);
			e->TargetNode = graph->Nodes + target;
			e->Capasity = 0;
			if (target < source)
			{
				for (uint32_t e2 = 0; e2 < e->TargetNode->EdgeCount; e2++)
				{
					Edge* linkedEdge = e->TargetNode->Edges + e2;
					if (graph->Nodes + source == linkedEdge->TargetNode)
					{
						e->LinkedEdge = linkedEdge;
						linkedEdge->LinkedEdge = e;
						break;
					}
				}
			}
			e++;
		}
	}

		
	capacityGenerator(graph);

		
	for (uint32_t node = 0; node < graph->NodeCount; node++)
	{
		for (uint32_t edge = originalEdgeCount[node]; edge <= graph->Nodes[node].EdgeCount; edge++)
		{
			graph->Nodes[node].Edges[edge].Capasity = 0;
		}
	}
		

	delete[] edges;
	




	return graph;
}



UpdateFunc GraphGenerator::AdditionUpdate(uint32_t x)
{
	return [x](uint32_t n) -> uint32_t{ return x + n;};
}

UpdateFunc GraphGenerator::MultiplicationUpdate(uint32_t x)
{
	return [x](uint32_t n) -> uint32_t{ return x*n;};
}


uint32_t randomUint32()
{
	return (uint32_t)rand();
}

GenerateCapacitiesFunc GraphGenerator::RandomCapacityGenerator(uint32_t min, uint32_t max)
{
	return [=](Graph* graph) -> void
	{
		EdgeEnumerator enumerator(graph);
		while (enumerator.Next())
		{
			enumerator.GetCurrent()->Capasity = min + randomUint32()%(max - min);
		}
	};
}


GenerateGraphFunc GraphGenerator::GenerateGraphCustom(double tightness)
{
	return [=](uint32_t nodeCount, vector<uint32_t>* edges) -> void
	{
		//Add what is needed for tightness 0 (precicly one edge from each node, and connected => a path in G)
		for (int n = 0; n < nodeCount; n++)
			edges[n].push_back((n + 1)%nodeCount);

		uint32_t edgesRequired = tightness*(nodeCount-2)*nodeCount;

		if (edgesRequired == 0)
			return;
		
		pair<uint32_t, uint32_t>* remainingEdges = new pair<uint32_t, uint32_t>[nodeCount*(nodeCount - 2)];
		pair<uint32_t, uint32_t>* e = remainingEdges;
		for (int n1 = 0; n1 < nodeCount; n1++)
		for (int n2 = 0; n2 < nodeCount; n2++)
		{
			if (n1 == n2 || (n1 + 1)%nodeCount == n2)
				continue;
			e->first = n1;
			e->second = n2;
			e++;
		}
		random_shuffle(remainingEdges, remainingEdges + nodeCount*(nodeCount - 2));
		e = remainingEdges;
		for (int i = 0; i < edgesRequired; i++)
		{
			edges[e->first].push_back(e->second);
			e++;
		}
		delete[] remainingEdges;
	};
}


GenerateGraphFunc GraphGenerator::GenerateConnectedGraph()
{
	return [=](uint32_t nodeCount, vector<uint32_t>* edges) -> void
	{
		for (int n1 = 1; n1 < nodeCount - 1; n1++)
		{
			edges[0].push_back(n1);
			edges[n1].push_back(nodeCount - 1);
			for (int n2 = 1; n2 < nodeCount - 1; n2++)
			{
				if (n1 != n2)
					edges[n1].push_back(n2);
			}
		}
	};
}

GenerateCapacitiesFunc GraphGenerator::ConnectedCapacityGenerator(uint32_t min, uint32_t max, bool worstCase)
{
	return [=](Graph* graph) -> void
	{
		Node* source = graph->Nodes;
		Node* target = graph->Nodes + graph->NodeCount - 1;

		Edge** edgesToTarget = new Edge*[graph->NodeCount - 2];
		int i = 0;
		
		uint32_t capOutOfSource = 0;
		uint32_t capIntoTarget = 0;

		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
		{
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (node == source || edge->TargetNode == target)
				{
					edge->Capasity = min + randomUint32()%(max - min);
					if (edge->TargetNode == target)
					{
						edgesToTarget[i++] = edge;
						capIntoTarget += edge->Capasity;
					}
					else
						capOutOfSource += edge->Capasity;
				}
				else
					edge->Capasity = max;
			}
		}

		if ((capIntoTarget < capOutOfSource) != worstCase)
		{
			for (Edge* edge = source->Edges; edge < source->Edges + source->EdgeCount; edge++)
			{
				Edge* edge2 = edgesToTarget[edge - source->Edges];

				uint32_t temp = edge->Capasity;
				edge->Capasity = edge2->Capasity;
				edge2->Capasity = temp;
			}
		}
	};
}


GenerateGraphFunc GraphGenerator::GenerateLayeredGraph(int layers, double tightness)
{
	return [=](uint32_t nodeCount, vector<uint32_t>* edges) -> void
	{
		if ((nodeCount - 2)%layers != 0)
			throw "Error";
		int nodesPerLayer = (nodeCount - 2)/layers;
		for (int n = 1; n < nodeCount - 1; n++)
		{
			int layer = (n - 1)/nodesPerLayer;
			int firstNodeInThisLayer = 1 + layer*nodesPerLayer;
			int nodeInNextLayer = 1 + (layer + 1)*nodesPerLayer;
			if (nodeInNextLayer < nodeCount - 1)
				nodeInNextLayer += n - firstNodeInThisLayer;
			edges[n].push_back(nodeInNextLayer);
		}
		for (int n = 0; n < nodesPerLayer; n++)
		{
			edges[0].push_back(n + 1);
		}

		uint32_t minEdgeCount = nodesPerLayer //Edges from source
								+ nodeCount - 2; //Single edge from all nodes except sink
			
		uint32_t maxEdgeCount = nodesPerLayer //Edges from source
								+ nodesPerLayer  //Edges to sink
								+ layers*nodesPerLayer*(nodesPerLayer - 1) //Edges inside a layer
								+ (layers - 1)*nodesPerLayer*nodesPerLayer; //Edges to next layer


		uint32_t edgesRequired = tightness*maxEdgeCount + (1 - tightness)*minEdgeCount - minEdgeCount;

		if (edgesRequired == 0)
			return;
		
		pair<uint32_t, uint32_t>* remainingEdges = new pair<uint32_t, uint32_t>[maxEdgeCount - minEdgeCount];
		pair<uint32_t, uint32_t>* e = remainingEdges;
		for (int n1 = 1; n1 < nodeCount - 1; n1++)
		for (int n2 = max(1, n1 - nodesPerLayer); n2 < nodeCount - 1; n2++)
		{
			if (n1 == n2 || n2 == edges[n1].at(0))
				continue;
			int n1Layer = (n1 - 1)/nodesPerLayer;
			int n2Layer = (n2 - 1)/nodesPerLayer;
			if (n2Layer < n1Layer)
				continue;
			if (n1Layer + 1 < n2Layer)
				break;

			e->first = n1;
			e->second = n2;
			e++;
		}
		random_shuffle(remainingEdges, remainingEdges + maxEdgeCount - minEdgeCount);
		e = remainingEdges;
		for (int i = 0; i < edgesRequired; i++)
		{
			edges[e->first].push_back(e->second);
			e++;
		}
		delete[] remainingEdges;
	};
}