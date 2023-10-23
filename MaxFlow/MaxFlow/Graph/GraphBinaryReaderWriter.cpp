#include "Graph.h";

#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>

using namespace std;

void Write(ofstream& writer, uint32_t value)
{
	writer.write((const char*)&value, sizeof(uint32_t));
}


void Graph::SaveBin(const char* filename)
{
	remove(filename);
	
	ofstream writer;
	writer.open(filename, ios::binary);
	Write(writer, NodeCount);
	for (Node* node = Nodes; node < Nodes + NodeCount; node++)
	{
		Write(writer, node->EdgeCount);
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			Write(writer, edge->Capasity);
			Write(writer, edge->TargetNode - Nodes);
		}
	}
	writer.flush();
	writer.close();
}

uint32_t Read(ifstream& reader)
{
	uint32_t value;
	reader.read((char*)&value, sizeof(uint32_t));
	return value;
}

Graph* Graph::LoadBin(const char* filename)
{
	Graph* graph = new Graph();
	
	ifstream reader;
	reader.open(filename, ios::binary);

	graph->NodeCount = Read(reader);
	graph->Nodes = new Node[graph->NodeCount];
	graph->EdgeCount = 0;
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		node->EdgeCount = Read(reader);
		node->Edges = new Edge[node->EdgeCount];
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			graph->EdgeCount++;
			edge->Capasity = Read(reader);
			edge->TargetNode = graph->Nodes + Read(reader);
			edge->LinkedEdge = 0;
		}
	}
	reader.close();
	
	for (Node* sourceNode = graph->Nodes; sourceNode < graph->Nodes + graph->NodeCount; sourceNode++)
	{
		for (Edge* sourceEdge = sourceNode->Edges; sourceEdge < sourceNode->Edges + sourceNode->EdgeCount; sourceEdge++)
		{
			if (sourceEdge->LinkedEdge != 0)
				continue;
			Node* targetNode = sourceEdge->TargetNode;
			for (Edge* targetEdge = targetNode->Edges; targetEdge < targetNode->Edges + targetNode->EdgeCount; targetEdge++)
			{
				if (targetEdge->TargetNode == sourceNode)
				{
					targetEdge->LinkedEdge = sourceEdge;
					sourceEdge->LinkedEdge = targetEdge;
					break;
				}
			}
		}

	}
	return graph;
}