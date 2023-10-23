#pragma once
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include "../Graph/Graph.h"

using namespace boost;

class LibraryMaxFlowAlgorithm
{
public:
	virtual void Prepare(const char* filename) = 0;
	virtual uint32_t CalculateMaxFlow() = 0;
};


class LibraryPushRelabelAlgorithm : public LibraryMaxFlowAlgorithm
{
private:
		typedef adjacency_list_traits<vecS, vecS, directedS> Traits;
		typedef adjacency_list<vecS, vecS, directedS, 
		property<vertex_name_t, std::string>,
		property<edge_capacity_t, long,
			property<edge_residual_capacity_t, long,
		property<edge_reverse_t, Traits::edge_descriptor> > >
		> PRGraph;


		PRGraph g;
		Traits::vertex_descriptor s, t;
		property_map<PRGraph, edge_capacity_t>::type capacity;
		property_map<PRGraph, edge_reverse_t>::type rev;
		property_map<PRGraph, edge_residual_capacity_t>::type residual_capacity;
		  
public:


	virtual void Prepare(const char* filename)
	{
		std::ifstream file;
		file.open(filename, std::ios::in|std::ios::binary|std::ios::ate);
		file.seekg (0, ios::beg);

		capacity = get(edge_capacity, g);
		rev = get(edge_reverse, g);
		residual_capacity = get(edge_residual_capacity, g);

		read_dimacs_max_flow(g, capacity, rev, s, t, file);
		file.close();
	}
	
	virtual uint32_t CalculateMaxFlow()
	{
		return (uint32_t)push_relabel_max_flow(g, s, t);
	}
};

class LibraryEdmondsKarpAlgorithm : public LibraryMaxFlowAlgorithm
{
private:
		
  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
  typedef adjacency_list < listS, vecS, directedS,
    property < vertex_name_t, std::string >,
    property < edge_capacity_t, long,
    property < edge_residual_capacity_t, long,
    property < edge_reverse_t, Traits::edge_descriptor > > > > EKGraph;


		EKGraph g;
		Traits::vertex_descriptor s, t;
		property_map<EKGraph, edge_capacity_t>::type capacity;
		property_map<EKGraph, edge_reverse_t>::type rev;
		property_map<EKGraph, edge_residual_capacity_t>::type residual_capacity;
		  
public:


	virtual void Prepare(const char* filename)
	{
		std::ifstream file;
		file.open(filename, std::ios::in|std::ios::binary|std::ios::ate);
		file.seekg (0, ios::beg);

		capacity = get(edge_capacity, g);
		rev = get(edge_reverse, g);
		residual_capacity = get(edge_residual_capacity, g);

		read_dimacs_max_flow(g, capacity, rev, s, t, file);
		file.close();
	}
	
	virtual uint32_t CalculateMaxFlow()
	{
		return (uint32_t)edmonds_karp_max_flow(g, s, t);
	}
};


class LibraryBoykovKolmogorovAlgorithm : public LibraryMaxFlowAlgorithm
{
private:
		
  typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
  typedef adjacency_list < vecS, vecS, directedS,
    property < vertex_name_t, std::string,
    property < vertex_index_t, long,
    property < vertex_color_t, boost::default_color_type,
    property < vertex_distance_t, long,
    property < vertex_predecessor_t, Traits::edge_descriptor > > > > >,
    
    property < edge_capacity_t, long,
    property < edge_residual_capacity_t, long,
    property < edge_reverse_t, Traits::edge_descriptor > > > > BKGraph;


		BKGraph g;
		Traits::vertex_descriptor s, t;
		property_map<BKGraph, edge_capacity_t>::type capacity;
		property_map<BKGraph, edge_reverse_t>::type rev;
		property_map<BKGraph, edge_residual_capacity_t>::type residual_capacity;
		  
public:


	virtual void Prepare(const char* filename)
	{
		std::ifstream file;
		file.open(filename, std::ios::in|std::ios::binary|std::ios::ate);
		file.seekg (0, ios::beg);

		capacity = get(edge_capacity, g);
		rev = get(edge_reverse, g);
		residual_capacity = get(edge_residual_capacity, g);

		read_dimacs_max_flow(g, capacity, rev, s, t, file);
		file.close();
	}
	
	virtual uint32_t CalculateMaxFlow()
	{
		return (uint32_t)boykov_kolmogorov_max_flow(g, s, t);
	}
};


#include "Library\GridGraph_3D_6C.h"

class LibraryCommercial3D
{
private:
	uint32_t GetLengthOfPath(Graph* graph, Node* previousNode, Node* node)
	{
		uint32_t length = 2;
		for (; node->EdgeCount == 11; length++)
		{
			Node* nextNode = 0;
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->TargetNode != previousNode && edge->TargetNode != graph->Nodes && edge->TargetNode != graph->Nodes + graph->NodeCount - 1 && edge->TargetNode->EdgeCount <= 4)
				{
					nextNode = edge->TargetNode;
					break;
				}
			}
			node = nextNode;
		}
		return length;
	}

	struct GridCoordinates
	{
		uint32_t coords[3];
		bool isSet;
		bool isInQueue;
		uint32_t iteration;
		uint32_t edgeCount;
	};

public:
	void Prepare(Graph* graph)
	{
		Node* source = graph->Nodes;
		Node* target = graph->Nodes + graph->NodeCount - 1;
		//Assume input is a 3D grid graph
		//Find a node with 3 edges
		GridCoordinates* tags = new GridCoordinates[graph->NodeCount - 2];
		GridCoordinates* tag = tags;
		Node* corner = 0;
		uint32_t counts[30];
		for (int i = 0; i < 30; i++)
			counts[i] = 0;
		for (Node* node = graph->Nodes + 1; node < graph->Nodes + graph->NodeCount - 1; node++, tag++)
		{
			uint32_t edgeCount = node->EdgeCount;
			
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->TargetNode == source || edge->TargetNode == target || edge->Capasity == 0 && edge->LinkedEdge->Capasity == 0) 
					edgeCount--;
			}

			if (node->EdgeCount == 7 && corner == 0)
			{
				corner = node;
			}
			node->Tag = tag;
			tag->isSet = false;
			tag->edgeCount = edgeCount;
			counts[edgeCount]++;
		}


		uint32_t matches = 0;
		for (Node* node = graph->Nodes + 1; node < graph->Nodes + graph->NodeCount - 1; node++)
		{
			tag = node->GetTag<GridCoordinates*>();

			if (tag->edgeCount != 3)
				continue;
			bool match = true;
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->TargetNode == source || edge->TargetNode == target || edge->Capasity == 0 && edge->LinkedEdge->Capasity == 0) 
					continue;
				if (edge->TargetNode->GetTag<GridCoordinates*>()->edgeCount != 4)
					match = false;
			}
			if (!match)
				break;
			matches++;
			corner = node;
		}


		//uint32_t width = GetLengthOfPath(graph, corner, corner->Edges[0].TargetNode);
		//uint32_t height = GetLengthOfPath(graph, corner, corner->Edges[1].TargetNode);
		//uint32_t debth = GetLengthOfPath(graph, corner, corner->Edges[2].TargetNode);

		
		LinkedList<Node*> queue;
		corner->GetTag<GridCoordinates*>()->coords[0] = 0;
		corner->GetTag<GridCoordinates*>()->coords[1] = 0;
		corner->GetTag<GridCoordinates*>()->coords[2] = 0;
		corner->GetTag<GridCoordinates*>()->isSet = true;
		corner->GetTag<GridCoordinates*>()->iteration = 0;
		
		int iteration = 0;
		for (Edge* edge = corner->Edges; edge < corner->Edges + corner->EdgeCount; edge++)
		{
			Node* node = edge->TargetNode;
			if (edge->TargetNode == source || edge->TargetNode == target || edge->Capasity == 0 && edge->LinkedEdge->Capasity == 0) 
				continue;
			
			node->GetTag<GridCoordinates*>()->coords[0] = 0;
			node->GetTag<GridCoordinates*>()->coords[1] = 0;
			node->GetTag<GridCoordinates*>()->coords[2] = 0;
			node->GetTag<GridCoordinates*>()->isSet = true;
			node->GetTag<GridCoordinates*>()->iteration = 1;
			
			node->GetTag<GridCoordinates*>()->coords[iteration++] = 1;
			
			for (Edge* edge2 = node->Edges; edge2 < node->Edges + node->EdgeCount; edge2++)
			{
				if (edge2->TargetNode == source || edge2->TargetNode == target) 
					continue;
				tag = edge2->TargetNode->GetTag<GridCoordinates*>();
				if (!tag->isSet)
				{
					tag->iteration = 2;
					tag->isInQueue = true;
					queue.AddFirst(edge2->TargetNode);
				}
			}
		}


		GridCoordinates** neighbors = new GridCoordinates*[26];
		
		while (!queue.IsEmpty())
		{
			Node* node = queue.RemoveLast();
			tag = node->GetTag<GridCoordinates*>();
			if (tag->isSet)
				continue;
			uint32_t neighborCount = 0;
			
			for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			{
				if (edge->TargetNode == source || edge->TargetNode == target) 
					continue;
				GridCoordinates* targetTag = edge->TargetNode->GetTag<GridCoordinates*>();
				if (targetTag->isSet)
				{
					if (targetTag->iteration == tag->iteration - 1)
						neighbors[neighborCount++] = targetTag;
				}
				else if (!targetTag->isInQueue)
				{
					queue.AddFirst(edge->TargetNode);
					targetTag->isInQueue = true;
					targetTag->iteration = tag->iteration + 1;
				}
			}

			/*if (neighborCount == 1 && node->EdgeCount > 4)
			{
				tag->iteration++;
				queue.AddFirst(node);
				continue;
			}*/

			
			tag->coords[0] = 0;
			tag->coords[1] = 0;
			tag->coords[2] = 0;
			tag->isSet = true;
			if (neighborCount == 1)
			{
				for (int i = 0; i < 3; i++)
				{
					if (neighbors[0]->coords[i] != 0)
					{
						tag->coords[i] = neighbors[0]->coords[i] + 1;
					}
				}
			}
			else
			{
				for (GridCoordinates** pNeighbor = neighbors; pNeighbor < neighbors + neighborCount; pNeighbor++)
				{
					GridCoordinates* neighbor = *pNeighbor;
					for (int i = 0; i < 3; i++)
					{
						if (neighbor->coords[i] > tag->coords[i])
						{
							tag->coords[i] = neighbor->coords[i];
						}
					}
				}
			}


		}


		//GridGraph_3D_6C* algorithm = new GridGraph_3D_6C(width, height, debth);


	}

	uint32_t CalculateMaxFlow()
	{
	}
};