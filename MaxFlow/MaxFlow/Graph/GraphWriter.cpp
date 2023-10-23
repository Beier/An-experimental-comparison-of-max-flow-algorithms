#include "Graph.h"
#include <fstream>
#include <iostream>

using namespace std;


uint32_t GetCapacity(Graph* graph, uint32_t i, uint32_t j)
{
	Node* sourceNode = graph->Nodes + i;
	Node* targetNode = graph->Nodes + j;
	for (i = 0; i < sourceNode->EdgeCount; i++)
	{
		if (sourceNode->Edges[i].TargetNode == targetNode)
			return sourceNode->Edges[i].Capasity;
	}
	return 0;
}

void Graph::SaveCsv(const char* filename)
{
	remove(filename);

	ofstream writer;
	writer.open(filename);
	for (uint32_t i = 0; i < NodeCount; i++)
	{
		if (i != 0)			
			writer << "\n";
		for (uint32_t j = 0; j < NodeCount; j++)
		{
			if (j != 0)
				writer << ';';
			uint32_t capacity = GetCapacity(this, i, j);
			if (capacity != 0)
				writer << capacity;
		}
	}
	writer.flush();
	writer.close();
}
