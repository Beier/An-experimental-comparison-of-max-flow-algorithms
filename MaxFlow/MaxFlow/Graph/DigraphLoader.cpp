

#include "Graph.h"
#include "../Utilities/LinkedList.h"

int GetLineLength(char* c)
{
	int count = 0;
	while (*c++ != '\n')
		count++;
	return count;
}

struct UIntPair
{
	uint32_t e1;
	uint32_t e2;
};

int ReadNextNumber(char*& p)
{
	while ('0' > *p || *p > '9')
		p++;
	char* end = p;
	while ('0' <= *end && *end <= '9')
		end++;
	char oldChar = *end;
	*end = 0;
	int returner = atoi(p);
	*end = oldChar;
	p = end;
	return returner;
}

void MapIndex(int& i, int map1From, int map1To, int map2From, int map2To)
{
	if (i == map1From)
		i = map1To;
	else if (i == map1To)
		i = map1From;
	else if (i == map2From)
		i = map2To;
	else if (i == map2To)
		i = map2From;
}

Graph* Graph::LoadDigraph(const char* filename)
{
	char* contents = Utils::LoadFile(filename);
	char* p = contents;

	/*
		c Comment
		p max      <nodeCount>        <edgeCount>
		n       1 s
		n      48 t
		a       1      31       2274
		a       1       2     160000
	*/
	
	Graph* graph = new Graph();
	int sourceIndex;
	int targetIndex;
	LinkedList<UIntPair*>* intermidiateNodes = 0;
	UIntPair* pairs = 0;
	UIntPair* pair;
	int edgesToAdd = 1;
	while (edgesToAdd > 0)
	{
		int length = GetLineLength(p);
		switch (*p)
		{
		case 'p':
			graph->NodeCount = ReadNextNumber(p);
			intermidiateNodes = new LinkedList<UIntPair*>[graph->NodeCount];
			graph->EdgeCount = ReadNextNumber(p);
			pairs = new UIntPair[graph->EdgeCount];
			pair = pairs;
			edgesToAdd = graph->EdgeCount;
			break;
		case 'n':
			int i = ReadNextNumber(p);
			p++;
			if (*p == 's')
				sourceIndex = i - 1;
			else if (*p == 't')
				targetIndex = i - 1;
			else
				throw "Error";
			break;
		case 'a':
			int from = ReadNextNumber(p) - 1;
			int to = ReadNextNumber(p) - 1;
			int capacity = ReadNextNumber(p);

			MapIndex(from, sourceIndex, 0, targetIndex, graph->NodeCount - 1);
			MapIndex(to, sourceIndex, 0, targetIndex, graph->NodeCount - 1);

			pair->e1 = to;
			pair->e2 = capacity;
			intermidiateNodes[from].AddFirst(pair);
			pair++;
			edgesToAdd--;
			break;
		default:
			//Skip line
			p += length + 1;
			break;
		}
		while (*(p - 1) != '\n')
			p++;
	}
	
	
	graph->Nodes = new Node[graph->NodeCount];

	for (int i = 0; i < graph->NodeCount; i++)
	{
		Node* node = graph->Nodes + i;
		node->EdgeCount = intermidiateNodes[i].GetCount();
		node->Edges = new Edge[node->EdgeCount];
		Edge* edge = node->Edges;
		for (LinkedList<UIntPair*>::Node* elm = intermidiateNodes[i].GetHead(); elm != 0; elm = elm->GetNext(), edge++)
		{
			edge->TargetNode = graph->Nodes + elm->GetInstance()->e1;
			edge->Capasity = elm->GetInstance()->e2;
			edge->LinkedEdge = 0;
			edge->Tag = 0;
		}
		intermidiateNodes[i].Clear();
	}

	pair = pairs;
	
	if (sqrtl(graph->EdgeCount) > graph->NodeCount/2)
	{
		Edge** linkedEdgeLookup = new Edge*[graph->NodeCount * graph->NodeCount];
		for (Edge** p = linkedEdgeLookup; p < linkedEdgeLookup + graph->NodeCount * graph->NodeCount; p++)
			*p = 0;

		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			int i1 = node - graph->Nodes;
			int i2 = edge->TargetNode - graph->Nodes;

			if (i2 < i1)
			{
				int temp = i1;
				i1 = i2;
				i2 = temp;
			}
			Edge** slot = linkedEdgeLookup + i1 + i2*graph->NodeCount;
			if (*slot == 0)
				*slot = edge;
			else
			{
				edge->LinkedEdge = *slot;
				edge->LinkedEdge->LinkedEdge = edge;
			}
		}
		delete[] linkedEdgeLookup;
	}
	else
	{
		for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			if (edge->LinkedEdge != 0)
				continue;
			for (Edge* linked = edge->TargetNode->Edges; linked < edge->TargetNode->Edges + edge->TargetNode->EdgeCount; linked++)
			{
				if (linked->TargetNode == node)
				{
					edge->LinkedEdge = linked;
					linked->LinkedEdge = edge;
					break;
				}
			}
		}
	}

	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
	{
		if (edge->LinkedEdge != 0)
			continue;
		pair->e1 = node - graph->Nodes;
		pair->e2 = edge - node->Edges;
		intermidiateNodes[edge->TargetNode - graph->Nodes].AddFirst(pair);
		pair++;
		graph->EdgeCount++;
	}
	
	for (int i = 0; i < graph->NodeCount; i++)
	{
		Node* node = graph->Nodes + i;
		if (intermidiateNodes[i].IsEmpty())
			continue;
		uint32_t oldEdgeCount = node->EdgeCount;
		node->EdgeCount += intermidiateNodes[i].GetCount();
		Edge* newEdgeArray = new Edge[node->EdgeCount];
		for (int j = 0; j < oldEdgeCount; j++)
		{
			newEdgeArray[j] = node->Edges[j];
			if (newEdgeArray[j].LinkedEdge != 0)
			{
				newEdgeArray[j].LinkedEdge->LinkedEdge = newEdgeArray + j;
			}
		}
		delete[] node->Edges;
		node->Edges = newEdgeArray;
		Edge* edge = node->Edges + oldEdgeCount;
		for (LinkedList<UIntPair*>::Node* elm = intermidiateNodes[i].GetHead(); elm != 0; elm = elm->GetNext(), edge++)
		{
			edge->TargetNode = graph->Nodes + elm->GetInstance()->e1;
			edge->Capasity = 0;
			edge->LinkedEdge = edge->TargetNode->Edges + elm->GetInstance()->e2;
			edge->LinkedEdge->LinkedEdge = edge;
			edge->Tag = 0;
		}
	}


	delete[] contents;
	delete[] pairs;
	delete[] intermidiateNodes;

	//graph->AssertConsistent();
	return graph;

}

#include <fstream>
#include <iostream>
using namespace std;

void Graph::SaveDigraph(const char* filename)
{
	
	/*
		c Comment
		p max      <nodeCount>        <edgeCount>
		n       1 s
		n      48 t
		a       1      31       2274
		a       1       2     160000
	*/

	remove(filename);

	ofstream writer;
	writer.open(filename);

	uint32_t nonZeroEdgeCount = 0;
	for (Node* node = Nodes; node < Nodes + NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			if (edge->Capasity > 0)
				nonZeroEdgeCount++;
		}
	}


	writer << "p max " << NodeCount << " " << nonZeroEdgeCount << "\n";
	writer << "n 1 s\n";
	writer << "n " << NodeCount << " t\n";
	
	for (Node* node = Nodes; node < Nodes + NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			if (edge->Capasity > 0)
				writer << "a " << node - Nodes + 1 << " " << edge->TargetNode - Nodes + 1 << " " << edge->Capasity << "\n";
		}
	}
	writer.flush();
	writer.close();
}