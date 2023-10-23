#include "Graph.h"

#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>

using namespace std;


bool ShouldAddEdge(vector<vector<uint32_t>*>* data, uint32_t from, uint32_t to, uint32_t& capacity);
vector<vector<uint32_t>*>* ParseFile(char* contents, Graph* graph);
void BuildGraph(vector<vector<uint32_t>*>* parsedContents, Graph* graph);
void Cleanup(char* contents, vector<vector<uint32_t>*>* parsedContents);
Graph* Graph::Load(const char* filename)
{
	Graph* graph = new Graph();
	char* contents = Utils::LoadFile(filename);
	vector<vector<uint32_t>*>* parsedContents = ParseFile(contents, graph);
	BuildGraph(parsedContents, graph);
	Cleanup(contents, parsedContents);
	return graph;
}

char* Utils::LoadFile(const char* filename)
{
	char* contents = 0;
	ifstream file;
	file.open(filename, ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		uint32_t length = (uint32_t)file.tellg();
		contents = new char [length + 1];
		file.seekg (0, ios::beg);
		file.read(contents, length);
		file.close();
		contents[length] = 0;
	}
	return contents;
}

uint32_t GetCapasity(vector<vector<uint32_t>*>* data, uint32_t from, uint32_t to)
{
	if (from >= data->size())
		return 0;
	vector<uint32_t>* row = data->at(from);
	if (to >= row->size())
		return 0;
	return row->at(to);
}


bool ShouldAddEdge(vector<vector<uint32_t>*>* data, uint32_t from, uint32_t to, uint32_t& capacity)
{
	capacity = GetCapasity(data, from, to);		
	return capacity > 0 || GetCapasity(data, to, from) > 0; 
}

vector<vector<uint32_t>*>* ParseFile(char* contents, Graph* graph)
{
	char* start = contents;
	char* p = start;

	vector<vector<uint32_t>*>* parsedContents = new vector<vector<uint32_t>*>();
	p = start;	
	vector<uint32_t>* row = new vector<uint32_t>();
	graph->NodeCount = 0;
	while (true)
	{
		if (*p == ';' || *p == '\r' || *p == '\n' || *p == 0)
		{
			if (p == start)
				row->push_back(0);
			else
				row->push_back(boost::lexical_cast<uint32_t>(start, p - start));
			start = p + 1;
		}
		if (*p == '\r' || *p == '\n' || *p == 0)
		{
			graph->NodeCount = max((size_t)graph->NodeCount, row->size());
			parsedContents->push_back(row);
			row = new vector<uint32_t>();
			if (*p == '\r')
			{
				p++;
				start = p + 1;
			}
		}
		if (*p == 0)
			break;
		p++;
	}
	delete row;
	graph->NodeCount = max((size_t)graph->NodeCount, parsedContents->size());

	return parsedContents;
}


void FixLinkedEdges(Graph* graph);
void BuildGraph(vector<vector<uint32_t>*>* parsedContents, Graph* graph)
{
	graph->Nodes = new Node[graph->NodeCount];
	vector<Edge*> edges;
	Edge* edge;
	for (uint32_t i = 0; i < graph->NodeCount; i++)
	{
		graph->Nodes[i].EdgeCount = 0;
		for (uint32_t j = 0; j < graph->NodeCount; j++)
		{
			uint32_t capacity;
			if (ShouldAddEdge(parsedContents, i, j, capacity))
			{
				edge = new Edge();
				edge->Capasity = capacity;
				edge->TargetNode = graph->Nodes + j;
				edges.push_back(edge);
				graph->Nodes[i].EdgeCount++;
				graph->EdgeCount++;
			}
		}
	}


	for (int i = graph->NodeCount - 1; i >= 0; i--)
	{
		graph->Nodes[i].Edges = new Edge[graph->Nodes[i].EdgeCount];
		if (graph->Nodes[i].EdgeCount > 0)
		for (int j = graph->Nodes[i].EdgeCount - 1; j >= 0; j--)
		{
			edge = edges.back();
			edges.pop_back();
			graph->Nodes[i].Edges[j] = *edge;
			delete edge;
		}
	}

	FixLinkedEdges(graph);
}


void FixLinkedEdges(Graph* graph)
{
	for (uint32_t i = 0; i < graph->NodeCount; i++)
	{
		for (uint32_t j = 0; j < graph->Nodes[i].EdgeCount; j++)
		{
			for (uint32_t k = 0; k < graph->Nodes[i].Edges[j].TargetNode->EdgeCount; k++)
			{
				if (&graph->Nodes[i] == graph->Nodes[i].Edges[j].TargetNode->Edges[k].TargetNode)
					graph->Nodes[i].Edges[j].LinkedEdge = &graph->Nodes[i].Edges[j].TargetNode->Edges[k];
			}

		}
	}
}

void Cleanup(char* contents, vector<vector<uint32_t>*>* parsedContents)
{
	delete[] contents;
	for (uint32_t i = 0; i < parsedContents->size(); i++)
		delete parsedContents->at(i);
	delete parsedContents;
}

