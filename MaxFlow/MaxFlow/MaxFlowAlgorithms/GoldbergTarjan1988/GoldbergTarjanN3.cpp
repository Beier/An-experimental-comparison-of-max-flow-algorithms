#include "GoldbergTarjanN3.h"
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "../../Utilities/Debug.h"

using namespace std;
#define PRINT_IMAGES 0
#define HIDE_NO_CAP_EDGES 0

uint32_t GoldbergTarjanN3::CalculateMaxFlow(Graph* graph, Node* source, Node* target)	//Discharge
{
	m_graph = graph;	
	this->source = source;
	this->target = target;
	Initialize();
	//cout << "    " << GetByteSize() << "B    ";

	Discharge(graph, source, target);
	
	uint32_t flow = target->GetTag<GTTag*>()->excess;
	CleanUp();
	return flow;
}

void GoldbergTarjanN3::Initialize()
{
	m_queue = new Queue<Node*>(m_graph->NodeCount);
	gttags = new GTTag[m_graph->NodeCount];


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
		e->TargetNode->GetTag<GTTag*>()->excess = startFlow;
		e->LinkedEdge->Capasity += startFlow;
		m_queue->Enqueue(e->TargetNode);
	}
}

void GoldbergTarjanN3::CleanUp()
{
	IsCleaning = true;
	delete[] gttags;
	delete m_queue;
}

void GoldbergTarjanN3::Discharge(Graph* graph, Node* source, Node* target)
{
	//PrintGraph(source, target, source);
	while(m_queue->GetSize() != 0)
	{
		Node* n = m_queue->Dequeue();
		Debug << "Working on node: " << graph->IndexOf(n) << "\n";

		if(n == source|| n == target)
			continue;
		uint32_t nodeExcess = ((GTTag*)n->Tag)->excess;
		uint32_t nodeDepth = ((GTTag*)n->Tag)->depth;
		while (nodeExcess != 0 && ((GTTag*)n->Tag)->depth <= nodeDepth)
		{
			//PrintGraph(source, target, n);
			PushRelabel(n);
			nodeExcess = ((GTTag*)n->Tag)->excess;
		}
		if(nodeExcess != 0)	//n still active
		{
			m_queue->Enqueue(n);
		}
		FinishProcessingNode(n);
	}
	//PrintGraph(source, target, target);
}

void GoldbergTarjanN3::PushRelabel( Node* v)
{
	Edge* curEdge = ((GTTag*)v->Tag)->currentEdge;
	Node* to = curEdge->TargetNode;

	if(curEdge->Capasity > 0 && ((GTTag*)v->Tag)->depth == ((GTTag*)to->Tag)->depth+1)	//Current Edge is push aplicable
	{
		Push(curEdge);
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
			Relabel(v);
		}
	}
}

void GoldbergTarjanN3::Push( Edge* e)
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

void GoldbergTarjanN3::Relabel(Node* n)
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

void GoldbergTarjanN3GlobalRename::Initialize()
{
	performingGlobalRelabel = true;
	GoldbergTarjanN3::Initialize();
	performingGlobalRelabel = false;
	globalRelabelQueue = new Queue<Node*>(m_graph->NodeCount);
	GlobalRelabel();
}

void GoldbergTarjanN3GlobalRename::CleanUp()
{
	IsCleaning = true;
	GoldbergTarjanN3::CleanUp();
	delete globalRelabelQueue;
}

void GoldbergTarjanN3GlobalRename::Relabel(Node* node)
{
	GTTag* tag = node->GetTag<GTTag*>();
	uint32_t oldLabel = tag->depth;
	GoldbergTarjanN3::Relabel(node);
	if (tag->depth - oldLabel > 1)
		GlobalRelabel();
}

void GoldbergTarjanN3GlobalRename::GlobalRelabel()
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

			if (neighborTag->globalRelabelIteration == currentIteration && edge->LinkedEdge->Capasity > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && edge->Capasity > 0)
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

			if (neighborTag->globalRelabelIteration == currentIteration && edge->LinkedEdge->Capasity > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && edge->Capasity > 0)
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



void GoldbergTarjanN3GlobalRename::RelabelTo(Node* node, uint32_t targetLabel)
{
	GTTag* tag = node->GetTag<GTTag*>();
	tag->currentEdge = node->Edges;
	tag->depth = targetLabel;
}


void GoldbergTarjanN3RareGlobalRename::Initialize()
{
	passEndNode = 0;
	excessLastPass = 0;
	excessLastRelabel = 0;
	GoldbergTarjanN3GlobalRename::Initialize();
}

void GoldbergTarjanN3RareGlobalRename::Relabel(Node* node)
{
	GoldbergTarjanN3::Relabel(node);
}

void GoldbergTarjanN3RareGlobalRename::GlobalRelabel()
{
	GoldbergTarjanN3GlobalRename::GlobalRelabel();
	GTTag* tag = target->GetTag<GTTag*>();
	passEndNode = m_queue->PeekEnd();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void GoldbergTarjanN3RareGlobalRename::FinishProcessingNode(Node* node)
{	
	if (passEndNode != 0 && node != passEndNode)
		return;
	
	GTTag* tag = target->GetTag<GTTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
	{
		//passEndNode = m_queue->PeekEnd();
		excessLastPass = tag->excess;
	}
}


void GoldbergTarjanN3RareGlobalRename2::Initialize()
{
	nodesProcessLimit = (uint32_t)(1.8956*pow(m_graph->NodeCount, 0.6548));
	GoldbergTarjanN3RareGlobalRename::Initialize();
}

void GoldbergTarjanN3RareGlobalRename2::SetCheckLimit(uint32_t limit)
{
	nodesProcessLimit = limit;
}

void GoldbergTarjanN3RareGlobalRename2::GlobalRelabel()
{
	GoldbergTarjanN3GlobalRename::GlobalRelabel();
	GTTag* tag = target->GetTag<GTTag*>();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void GoldbergTarjanN3RareGlobalRename2::FinishProcessingNode(Node* node)
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




	/*


#include <fstream>
#include <iostream>
#include <sstream> 

#include <stdio.h>  /* defines FILENAME_MAX *

#include <direct.h>
#define GetCurrentDir _getcwd


void GoldbergTarjanN3::PrintGraph(Node* source, Node* target, Node* currentNode)
{
	
	if(!PRINT_IMAGES)
		return;
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
}*/