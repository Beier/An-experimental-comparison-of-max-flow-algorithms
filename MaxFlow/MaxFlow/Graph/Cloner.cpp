#include "Graph.h"



Graph* Graph::Clone()
{
	Graph* clone = new Graph();

	clone->NodeCount = NodeCount;
	clone->EdgeCount = EdgeCount;
	clone->Nodes = new Node[NodeCount];
	
	for (int n = 0; n < NodeCount; n++)
	{
		clone->Nodes[n].EdgeCount = Nodes[n].EdgeCount;
		clone->Nodes[n].Edges = new Edge[Nodes[n].EdgeCount];
		clone->Nodes[n].Tag = 0;
	}
	for (int n = 0; n < NodeCount; n++)
	{
		for (int e = 0; e < Nodes[n].EdgeCount; e++)
		{
			clone->Nodes[n].Edges[e].Capasity = Nodes[n].Edges[e].Capasity;
			clone->Nodes[n].Edges[e].TargetNode = clone->Nodes + (Nodes[n].Edges[e].TargetNode - Nodes);
			clone->Nodes[n].Edges[e].LinkedEdge = clone->Nodes[n].Edges[e].TargetNode->Edges + (Nodes[n].Edges[e].LinkedEdge - Nodes[n].Edges[e].TargetNode->Edges);
			clone->Nodes[n].Edges[e].Tag = 0;
		}
	}
	if (!Equals(clone))
		throw "Error";
	return clone;
}

Node* Node::CloneShallow()
{
	Node* clone = new Node();
	clone->EdgeCount = EdgeCount;
	clone->Edges = new Edge[EdgeCount];
	clone->Tag = 0;

	for(int e = 0; e < EdgeCount; e++)
	{
		clone->Edges[e].Capasity = Edges[e].Capasity;
		clone->Edges[e].TargetNode = Edges[e].TargetNode;
		clone->Edges[e].LinkedEdge = Edges[e].LinkedEdge;
	}
	return clone;
}

Edge* Edge::CloneShallow()
{
	Edge* clone = new Edge();
	clone->Capasity = Capasity;
	clone->TargetNode = TargetNode;
	clone->LinkedEdge = LinkedEdge;
	return clone;
}