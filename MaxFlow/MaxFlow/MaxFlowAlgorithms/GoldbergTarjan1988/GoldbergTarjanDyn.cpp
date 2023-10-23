#include "GoldbergTarjanDyn.h"
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "../../Utilities/Debug.h"
#include "../../Utilities/UtilityFunctions.h"

using namespace std;
#define PRINT_IMAGES 0
#define HIDE_NO_CAP_EDGES 0

uint32_t GoldbergTarjanDyn::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	imageNr = 0;

	m_graph = graph;
	this->source = source;
	this->target = target;
	Initialize();

	//cout << "    " << GetByteSize() << "B    ";

	Discharge(source, target);

	TransferChangesFromDynamicTree();

	if(PRINT_IMAGES)
		PrintGraph(source, target, target);

	uint32_t flow = target->GetTag<GTTag*>()->excess;

	CleanUp();

	return flow;
}

void GoldbergTarjanDyn::Initialize()
{
	m_queue = new Queue<Node*>(m_graph->NodeCount);
	gttags = new GTTag[m_graph->NodeCount];
	m_forest = new DynamicForest<Node>(m_graph->Nodes, m_graph->NodeCount);

	m_maxSize = m_graph->NodeCount*m_graph->NodeCount/m_graph->EdgeCount;

	for(GTTag* tag = &gttags[0]; tag < gttags + m_graph->NodeCount; tag++)		//Initialize tags, including depth and excess
	{
		tag->depth=0;
		tag->excess=0;
		tag->globalRelabelIteration = 0;
	}
	

	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		n->Tag = &gttags[m_graph->IndexOf(n)];
		((GTTag*)n->Tag)->currentEdge = &n->Edges[0];
	}

	((GTTag*)source->Tag)->depth = m_graph->NodeCount;

	for(Edge* e = &source->Edges[0]; e < source->Edges+source->EdgeCount; e++)	//Send the max amount of flow out from the source
	{
		uint32_t startFlow = e->Capasity;
		if(startFlow == 0)
			continue;
		e->Capasity = 0;
		((GTTag*)e->TargetNode->Tag)->excess = startFlow;
		e->LinkedEdge->Capasity += startFlow;
		m_queue->Enqueue(e->TargetNode);
	}
}

void GoldbergTarjanDyn::CleanUp()
{
	IsCleaning = true;
	delete m_queue;
	delete[] gttags;
	delete m_forest;
	LinkedListNodeManager::ClearCache();
}


void GoldbergTarjanDyn::Discharge(Node* source, Node* target)
{
	if(PRINT_IMAGES)
		PrintGraph(source, target, source);
	while(m_queue->GetSize() != 0)
	{
		Node* n = m_queue->Dequeue();
		
		if(m_forest->GetRoot(n) != n) //Not a root
			continue;

		Debug << "Working on node: " << m_graph->IndexOf(n) << "\n";
		if(n == source|| n == target)
			continue;
		uint32_t nodeExcess = ((GTTag*)n->Tag)->excess;
		uint32_t nodeDepth = ((GTTag*)n->Tag)->depth;
		while (nodeExcess != 0 && ((GTTag*)n->Tag)->depth <= nodeDepth)
		{
			//if(PRINT_IMAGES)
			//	PrintGraph(source, target, n);
			TreePushRelabel(n);
			nodeExcess = ((GTTag*)n->Tag)->excess;
		}
		if(nodeExcess != 0)	//n still active
		{
			m_queue->Enqueue(n);
		}
		FinishProcessingNode(n);
	}
	if(PRINT_IMAGES)
		PrintGraph(source, target, target);

}

void GoldbergTarjanDyn::TransferChangesFromDynamicTree()
{
	for(Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		if(m_forest->GetLink(node) != NULL)
		{
			uint32_t treeCost = m_forest->GetCost(node);
			Edge* pushEdge = node->GetTag<GTTag*>()->pushEdge;
			uint32_t changedCap = pushEdge->Capasity - treeCost;
			pushEdge->Capasity = treeCost;
			pushEdge->LinkedEdge->Capasity += changedCap;
		}
	}
}

void GoldbergTarjanDyn::TreePushRelabel( Node* v)
{
	Edge* curEdge = ((GTTag*)v->Tag)->currentEdge;
	Node* to = curEdge->TargetNode;

	if(curEdge->Capasity > 0 && ((GTTag*)v->Tag)->depth == ((GTTag*)to->Tag)->depth+1)	//Current Edge is push aplicable
	{
		if(m_forest->GetPathLength(v) + m_forest->GetPathLength(to) <= m_maxSize && m_forest->GetLink(v) == NULL && m_forest->GetRoot(to) != v)
		{
				m_forest->Link(v, to);
				m_forest->SetCost(v, curEdge->Capasity);
				v->GetTag<GTTag*>()->pushEdge = curEdge;
				Send(v);
		}
		else
		{
			Push(curEdge);
			Send(to);
		}
	}
	else
	{
		if(v->IndexOfEdge(curEdge) < v->EdgeCount-1)	  //Not the last not null edge in the edge list
		{
			((GTTag*)v->Tag)->currentEdge++;
		}
		else
		{
			((GTTag*)v->Tag)->currentEdge = &v->Edges[0];
			Node* childNode;
			while((childNode = m_forest->GetFirstChild(v)) != NULL)
			{
				m_forest->Cut(childNode);
				Edge* cutEdge = childNode->GetTag<GTTag*>()->pushEdge;
				uint32_t treeValue = m_forest->GetCost(childNode);
				uint32_t newFlow = cutEdge->Capasity - treeValue;
				cutEdge->LinkedEdge->Capasity += newFlow;
				cutEdge->Capasity = treeValue;
			}
			Relabel(v);
		}
	}
}

void GoldbergTarjanDyn::Send( Node* v)
{
	GTTag* vTag = (GTTag*)v->Tag;
	while(m_forest->GetRoot(v) != v && vTag->excess > 0)
	{
		Node* cutNode = m_forest->GetMinBoundingNode(v);
		int delta = UtilityFunctions::min(vTag->excess, m_forest->GetCost(cutNode));
		m_forest->AddCost(v, -delta);
		vTag->excess -= delta;

		Node* root = m_forest->GetRoot(v);
		GTTag* rootTag = root->GetTag<GTTag*>();
		if(rootTag->excess == 0 && delta > 0)
			m_queue->Enqueue(root);
		rootTag->excess += delta;

		while(m_forest->GetCost(cutNode) == 0 && m_forest->GetLink(cutNode) != NULL)
		{
			m_forest->Cut(cutNode);
			Edge* cutEdge = cutNode->GetTag<GTTag*>()->pushEdge;
			cutEdge->LinkedEdge->Capasity += cutEdge->Capasity;
			cutEdge->Capasity = 0;
			if(m_forest->GetLink(v) == NULL)
				break;
			cutNode = m_forest->GetMinBoundingNode(v);
		}
	}
}



void GoldbergTarjanDyn::Push( Edge* e)
{
	Node* from = e->LinkedEdge->TargetNode;
	Node* to = e->TargetNode;
	uint32_t delta = min(e->Capasity, ((GTTag*)from->Tag)->excess);
	e->Capasity -= delta;

	e->LinkedEdge->Capasity += delta;
	((GTTag*)from->Tag)->excess -= delta;
	if(((GTTag*)to->Tag)->excess == 0)	//The to node becomes active
	{
		m_queue->Enqueue(to);
	}
	((GTTag*)to->Tag)->excess += delta;
}

void GoldbergTarjanDyn::Relabel(Node* n)
{
	uint32_t min = UINT32_MAX;
	for(uint32_t i = 0; i < n->EdgeCount; i++)
	{
		if(n->Edges[i].Capasity==0)
			continue;
		if(((GTTag*)n->Edges[i].TargetNode->Tag)->depth < min)
			min = ((GTTag*)n->Edges[i].TargetNode->Tag)->depth;
	}
	if(min != UINT32_MAX)
		min++;
	((GTTag*)n->Tag)->depth = min;
}

void GoldbergTarjanDyn::calcCap(Node* n, uint32_t& inGoingCap, uint32_t& outGoingCap)
{
	for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
	{
		Node* targetNode = e->TargetNode;
		if(m_forest->GetLink(targetNode) == n)
		{
			uint32_t treeCost = m_forest->GetCost(targetNode);
			inGoingCap += treeCost;
			outGoingCap += e->Capasity + e->LinkedEdge->Capasity - treeCost;

		} else if(m_forest->GetLink(n) == targetNode)
		{
			uint32_t treeCost = m_forest->GetCost(n);
			outGoingCap += treeCost;
			inGoingCap += e->LinkedEdge->Capasity + e->Capasity - treeCost;
		}
		else
		{
			inGoingCap += e->LinkedEdge->Capasity;
			outGoingCap += e->Capasity;
		}
	}
}

bool GoldbergTarjanDyn::AssertExcessHolds(Node* n, uint32_t inGoingCap, uint32_t outGoingCap)
{
	uint32_t newOutGoingCap = 0;
	uint32_t newInGoingCap = 0;
	calcCap(n, newInGoingCap, newOutGoingCap);
	if(inGoingCap + outGoingCap != newOutGoingCap + newInGoingCap)
	{
		return false;
	}
	return true;
}

bool GoldbergTarjanDyn::AssertCorrectPushEdge(Node* n)
{
	if(m_forest->GetLink(n) == NULL)
		return true;

	for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
	{
		Node* targetNode = e->TargetNode;
		if(m_forest->GetLink(n) == targetNode)
		{
			if(n->GetTag<GTTag*>()->pushEdge == e)
				return true;
			return false;
		}
	}
}


void GoldbergTarjanDynGlobalRename::Initialize()
{
	performingGlobalRelabel = true;
	GoldbergTarjanDyn::Initialize();
	performingGlobalRelabel = false;
	globalRelabelQueue = new Queue<Node*>(m_graph->NodeCount);
	GlobalRelabel();
}

void GoldbergTarjanDynGlobalRename::CleanUp()
{
	IsCleaning = true;
	GoldbergTarjanDyn::CleanUp();
	delete globalRelabelQueue;
}

void GoldbergTarjanDynGlobalRename::Relabel(Node* node)
{
	GTTag* tag = node->GetTag<GTTag*>();
	uint32_t oldLabel = tag->depth;
	GoldbergTarjanDyn::Relabel(node);
	if (tag->depth - oldLabel > 1)
		GlobalRelabel();
}


uint32_t GoldbergTarjanDynGlobalRename::GetCapacity(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	uint32_t capacity = edge->Capasity;
	if (m_forest->IsLinked(node) && edge->TargetNode == m_forest->GetLink(node))
		capacity = m_forest->GetCost(node);
	if (m_forest->IsLinked(edge->TargetNode) && node == m_forest->GetLink(edge->TargetNode))
		capacity += edge->LinkedEdge->Capasity - m_forest->GetCost(edge->TargetNode);
	return capacity;
}

void GoldbergTarjanDynGlobalRename::GlobalRelabel()
{
	if (performingGlobalRelabel)
		return;
	performingGlobalRelabel = true;

	globalRelabelQueue->Enqueue(target);

	GTTag* tag = target->GetTag<GTTag*>();
	uint32_t currentIteration = tag->globalRelabelIteration;

	tag->globalRelabelIteration++;

	//Labels from target
	while (!globalRelabelQueue->IsEmpty())
	{
		Node* node = globalRelabelQueue->Dequeue();
		tag = node->GetTag<GTTag*>();
		tag->globalRelabelIteration++;
		
		if (node == source)
			continue;

		uint32_t requiredLabel = 2*m_graph->NodeCount;
		if (node == target)
			requiredLabel = 0;
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			Node* neighbor = edge->TargetNode;
			GTTag* neighborTag = neighbor->GetTag<GTTag*>();

			if (neighborTag->globalRelabelIteration == currentIteration && GetCapacity(edge->LinkedEdge) > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && GetCapacity(edge) > 0)
			{
				if (requiredLabel > neighborTag->depth + 1)
					requiredLabel = neighborTag->depth + 1;
			}
		}
		
		if (requiredLabel > tag->depth)
			RelabelTo(node, requiredLabel);
	}

	globalRelabelQueue->Enqueue(source);
	tag = source->GetTag<GTTag*>();
	tag->globalRelabelIteration = currentIteration + 1;
	
	//Labels from source
	while (!globalRelabelQueue->IsEmpty())
	{
		Node* node = globalRelabelQueue->Dequeue();
		tag = node->GetTag<GTTag*>();
		tag->globalRelabelIteration++;
		
		if (node == target)
			continue;

		uint32_t requiredLabel = 2*m_graph->NodeCount;
		if (node == source)
			requiredLabel = m_graph->NodeCount;
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			Node* neighbor = edge->TargetNode;
			GTTag* neighborTag = neighbor->GetTag<GTTag*>();

			if (neighborTag->globalRelabelIteration == currentIteration && GetCapacity(edge->LinkedEdge) > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && GetCapacity(edge) > 0)
			{
				if (requiredLabel > neighborTag->depth + 1)
					requiredLabel = neighborTag->depth + 1;
			}
		}
		
		
		if (requiredLabel > tag->depth)
			RelabelTo(node, requiredLabel);
	}
	
	
	performingGlobalRelabel = false;
}


void GoldbergTarjanDynGlobalRename::RelabelTo(Node* node, uint32_t targetLabel)
{
	GTTag* tag = node->GetTag<GTTag*>();
	tag->currentEdge = node->Edges;
	Node* childNode;
	while((childNode = m_forest->GetFirstChild(node)) != NULL)
	{
		m_forest->Cut(childNode);
		Edge* cutEdge = childNode->GetTag<GTTag*>()->pushEdge;
		uint32_t treeValue = m_forest->GetCost(childNode);
		uint32_t newFlow = cutEdge->Capasity - treeValue;
		cutEdge->LinkedEdge->Capasity += newFlow;
		cutEdge->Capasity = treeValue;
	}
	tag->depth = targetLabel;
}



void GoldbergTarjanDynRareGlobalRename::Initialize()
{
	passEndNode = 0;
	excessLastPass = 0;
	excessLastRelabel = 0;
	GoldbergTarjanDynGlobalRename::Initialize();
}

void GoldbergTarjanDynRareGlobalRename::Relabel(Node* node)
{
	GoldbergTarjanDyn::Relabel(node);
}

void GoldbergTarjanDynRareGlobalRename::GlobalRelabel()
{
	GoldbergTarjanDynGlobalRename::GlobalRelabel();
	GTTag* tag = target->GetTag<GTTag*>();
	passEndNode = m_queue->PeekEnd();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void GoldbergTarjanDynRareGlobalRename::FinishProcessingNode(Node* node)
{
	if (passEndNode != 0 && node != passEndNode)
		return;
	
	GTTag* tag = target->GetTag<GTTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
	{
		passEndNode = m_queue->PeekEnd();
		excessLastPass = tag->excess;
	}
}




void GoldbergTarjanDynRareGlobalRename2::Initialize()
{
	nodesProcessLimit = (uint32_t)(0.54*pow(m_graph->NodeCount, 0.7144));
	GoldbergTarjanDynRareGlobalRename::Initialize();
}

void GoldbergTarjanDynRareGlobalRename2::SetCheckLimit(uint32_t limit)
{
	nodesProcessLimit = limit;
}

void GoldbergTarjanDynRareGlobalRename2::GlobalRelabel()
{
	GoldbergTarjanDynGlobalRename::GlobalRelabel();
	GTTag* tag = target->GetTag<GTTag*>();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void GoldbergTarjanDynRareGlobalRename2::FinishProcessingNode(Node* node)
{	
	nodesProcessed++;
	if (nodesProcessed < nodesProcessLimit)
		return;
	nodesProcessed = 0;
	GTTag* tag = target->GetTag<GTTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
		excessLastPass = tag->excess;
}


uint32_t GoldbergTarjanDyn::GetByteSize()
{
	return sizeof(GoldbergTarjanDyn) + m_queue->GetByteSize() + m_forest->GetByteSize() + m_graph->NodeCount*sizeof(GTTag) + m_graph->GetByteSize();
}

uint32_t GoldbergTarjanDynGlobalRename::GetByteSize()
{
	return GoldbergTarjanDyn::GetByteSize() - sizeof(GoldbergTarjanDyn) + sizeof(GoldbergTarjanDynGlobalRename) + globalRelabelQueue->GetByteSize();
}

uint32_t GoldbergTarjanDynRareGlobalRename::GetByteSize()
{
	return GoldbergTarjanDynGlobalRename::GetByteSize() - sizeof(GoldbergTarjanDynGlobalRename) + sizeof(GoldbergTarjanDynRareGlobalRename);
}

#include <fstream>
#include <iostream>
#include <sstream> 

#include <stdio.h>  /* defines FILENAME_MAX */

#include <direct.h>
#define GetCurrentDir _getcwd


void GoldbergTarjanDyn::PrintGraph(Node* source, Node* target, Node* currentNode)
{
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");
	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node \nColors:\ngreen - admissible edge\"";
	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		uint32_t id = m_graph->IndexOf(n);
		GTTag* tag = n->GetTag<GTTag*>();
		writer << "\"Node: " << id + 1 << "\"\n[";
		writer << "label=\"Distance: " << tag->depth << "\nExcess: "<< tag->excess << "\"";
		if(n == currentNode)
			writer << ",style=filled";		
		if (n == source)
			writer << ",color=green";		
		if (n == target)
			writer << ",color=red";
		writer << "];";
	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			//if (edge->Capasity > 0)
			{
				writer << "\"Node: " << m_graph->IndexOf(node) + 1;
				writer << "\"->\"Node: " << m_graph->IndexOf(edge->TargetNode) + 1;
				writer << "\"[label=\"" << edge->Capasity << "\"";
				if(edge->Capasity == 0 && HIDE_NO_CAP_EDGES)
					writer << ", color=white, arrowhead=none, fontcolor = white";
				else if (node->GetTag<GTTag*>()->depth > edge->TargetNode->GetTag<GTTag*>()->depth)
					writer << ", color=green";
				writer  << "];";
			}
		}
	}
	writer<<'}';
	writer.flush();
	writer.close();
	
	stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\DistanceGraph" << imageNr++ << ".jpg";

	int r = system(ss.str().c_str());
}