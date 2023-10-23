#include "Graph.h"


uint32_t Node::IndexOfEdge(Edge* e)
{
	return e-Edges;
}

bool Graph::SwapEdges(Edge* e1, Edge* e2)
{
	if(e1->LinkedEdge->TargetNode != e2->LinkedEdge->TargetNode)
		throw std::exception("Both edges is not from the same node");
	if (e1 == e2)
		return false;
	Edge temp = *e1;
	*e1 = *e2;
	*e2 = temp;
	e1->LinkedEdge->LinkedEdge = e1;
	e2->LinkedEdge->LinkedEdge = e2;
	return true;
}

bool Graph::SwapEdgesAndPointers(Edge** e1, Edge** e2)
{
	bool swapped = SwapEdges(*e1, *e2);
	if (swapped)
	{
		Edge* temp = *e1;
		*e1 = *e2;
		*e2 = temp;
	}
	return swapped;
}

uint32_t Graph::IndexOf(Node* n)
{
	return n-Nodes;
}

int graphEdgeSortComparator(const void * a, const void * b)
{
	Edge* edge1 = (Edge*)a;	
	Edge* edge2 = (Edge*)b;

	return edge1->TargetNode - edge2->TargetNode;
}

void Graph::SortEdges()
{
	for (Node* node = Nodes; node < Nodes + NodeCount; node++)
	{
		qsort(node->Edges, node->EdgeCount, sizeof(Edge), graphEdgeSortComparator);
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
			edge->LinkedEdge->LinkedEdge = edge;
	}
}


//uint32_t Graph::IndexOf(Node* n)
//{
//	return 3;
//}=======
//}

bool Graph::IsConsistent()
{
	try
	{
		AssertConsistent();
		return true;
	}
	catch (void* e)
	{
		return false;
	}
}

void Graph::AssertConsistent()
{
	Node* node = Nodes;
	for (uint32_t i = 0; i < NodeCount; i++, node++)
	{
		Edge* edge = node->Edges;
		for (uint32_t i2 = 0; i2 < node->EdgeCount; i2++, edge++)
		{
			if (edge->Capasity < 0)
				throw std::exception("Negative capacity");
			if (edge->LinkedEdge->TargetNode != node)
				throw std::exception("Linked edge does not point back");
			Node* targetNode = edge->TargetNode;
			Edge* backEdge = targetNode->Edges;
			bool foundLinkedEdge = false;
			for (uint32_t i3 = 0; i3 < targetNode->EdgeCount; i3++, backEdge++)
			{
				if (backEdge == edge->LinkedEdge)
				{
					foundLinkedEdge = true;
					break;
				}
			}
			if (!foundLinkedEdge)
				throw  std::exception("Linked edge not found in target node");
			if (backEdge->LinkedEdge != edge)
				throw  std::exception("Linked edge not linked to edge");
		}
	}
}

uint32_t Graph::GetMaxCapacity()
{
	uint32_t result = 0;
	for(Node* n = Nodes; n < Nodes + NodeCount; n++)
	{
		for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			result = std::max(result, e->Capasity);
		}
	}
	return result;
}


bool Graph::Equals(Graph* other)
{
	if (NodeCount != other->NodeCount)
		return false;
	if (EdgeCount != other->EdgeCount)
		return false;
	for (int n = 0; n < NodeCount; n++)
	{
		Node* n1 = Nodes + n;
		Node* n2 = other->Nodes + n;
		if (n1->EdgeCount != n2->EdgeCount)
			return false;
		for (int e = 0; e < n1->EdgeCount; e++)
		{
			Edge* e1 = n1->Edges + e;
			Edge* e2 = n2->Edges + e;

			if (e1->Capasity != e2->Capasity)
				return false;
			if (e1->TargetNode - Nodes != e2->TargetNode - other->Nodes)
				return false;
			if (e1->LinkedEdge - e1->TargetNode->Edges != e2->LinkedEdge - e2->TargetNode->Edges)
				return false;
		}
	}
	return true;	
}