#include "ModifiedKRT.h"

int ModifiedKRT::edgeComparator(const void * a, const void * b)
{
	Edge* edge1 = (Edge*)a;
	Edge* edge2 = (Edge*)b;

	int64_t edge1Capacity = edge1->Capasity + edge1->LinkedEdge->Capasity;
	int64_t edge2Capacity = edge2->Capasity + edge2->LinkedEdge->Capasity;

	int cmp = edge2Capacity - edge1Capacity;
	if (cmp != 0)
		return cmp;

	Node* node1 = edge1->LinkedEdge->TargetNode;
	Node* node2 = edge2->LinkedEdge->TargetNode;
	if (node1 != node2)
		return node1 - node2;

	return edge1 - edge2;
}


int ModifiedKRT::edgepComparator(const void * a, const void * b)
{
	Edge** edge1 = (Edge**)a;
	Edge** edge2 = (Edge**)b;
	return edgeComparator(*edge1, *edge2);
}

//O(nm)
void ModifiedKRT::Initialize()
{	
	float edgeRatio = sqrtf(graph->EdgeCount / graph->NodeCount);
	r0 = 1.0f/edgeRatio;
	l = edgeRatio;
	t = 1;
	float r = r0*2;
	while (r < 1)
	{
		t++;
		r*=2;
	}

	nodeTags = new NodeTag[graph->NodeCount];
	edgeTags = new EdgeTag[graph->EdgeCount];
	LinkedList<EdgeTag*>::Allocate(graph->EdgeCount*4);
	NodeTag* nodeTag = nodeTags;
	for (Node* n = graph->Nodes; n < graph->Nodes + graph->NodeCount; n++)
	{
		n->Tag = nodeTag;
		nodeTag->source.currentEdge = 0;
		nodeTag->source.activeEdgesByErl = new LinkedList<EdgeTag*>[t + 1];
		nodeTag->target.erl = 0;
		nodeTag->target.designatedEdges = 0;
		nodeTag->target.rl = 0;
		nodeTag->lable = 0;
		nodeTag->excess = 0;
		nodeTag->hiddenCapacities = 0;
		nodeTag->visibleExcess = 0;
		nodeTag->globalRelabelIteration = 0;
		nodeTag++;
	}

	EdgeTag* edgeTag = edgeTags;
	for (Node* n = graph->Nodes; n < graph->Nodes + graph->NodeCount; n++)
	{
		NodeTag* nodeTag = n->GetTag<NodeTag*>();
		for (Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			edgeTag->edge = e;
			e->Tag = edgeTag;
			edgeTag->sourceElm = 0;
			edgeTag->sourceErlElm = 0;
			edgeTag->targetElm = 0;
			edgeTag->isAdded = false;
			nodeTag->hiddenCapacities += e->Capasity;
			if (e->Capasity > 0)
				edgeTag->sourceElm = nodeTag->source.levelInegibleEdges.AddFirst(edgeTag);
			edgeTag++;
		}
	}
	PrintGraph(source, 0);
	//Push from source
	for (Edge* e = source->Edges; e < source->Edges + source->EdgeCount; e++)
	{
		AddEdge(e);
	}
	Relable(source);
	for (Edge* e = source->Edges; e < source->Edges + source->EdgeCount; e++)
	{
		Push(e, e->Capasity);
	}
	while (source->GetTag<NodeTag*>()->lable < graph->NodeCount)
	{
		Relable(source);
	}

	edgesToInsert = new Edge*[graph->EdgeCount/2 - source->EdgeCount];
	Edge** nextEdgePos = edgesToInsert;
	for (Node* n = graph->Nodes; n < graph->Nodes + graph->NodeCount; n++)
	{
		if (n == source)
			continue;
		for (Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			if (e->TargetNode == source || e->TargetNode < n)
				continue;
			*nextEdgePos++ = e;
		}
	}
	qsort(edgesToInsert, graph->EdgeCount/2 - source->EdgeCount, sizeof(Edge*), edgepComparator);
}

//O(m)
void ModifiedKRT::CleanUp()
{
	IsCleaning = true;
	for (NodeTag* tag = nodeTags; tag < nodeTags + graph->NodeCount; tag++)
		delete[] tag->source.activeEdgesByErl;
	delete[] nodeTags;
	delete[] edgeTags;
	delete[] edgesToInsert;
	nodeTags = 0;
	edgeTags = 0;
	edgesToInsert = 0;
	LinkedListNodeManager::ClearCache();
}


uint32_t ModifiedKRT::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	this->graph = graph;
	this->source = source;
	this->target = target;
	Initialize();
	for (int i = 0; i < graph->EdgeCount/2 - source->EdgeCount; i++)
	{
		//PrintGraph(0, 0);
		Edge* edge = edgesToInsert[i];
		AddEdge(edge);

		
		Node* source = edge->LinkedEdge->TargetNode;
		Node* target = edge->TargetNode;
		
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		NodeTag* targetTag = target->GetTag<NodeTag*>();
		


		if (targetTag->lable < sourceTag->lable && edge->Capasity <= sourceTag->visibleExcess)
			Push(edge, edge->Capasity);
		else if (sourceTag->lable < targetTag->lable && edge->LinkedEdge->Capasity <= targetTag->visibleExcess)
			Push(edge->LinkedEdge, edge->LinkedEdge->Capasity);

		while (!readyNodes.IsEmpty())
		{
			Node* node = readyNodes.RemoveFirst();
			NodeTag* nodeTag = node->GetTag<NodeTag*>();
			if (node == this->source || node == this->target)
				continue;
			while (nodeTag->visibleExcess > 0)
			{
				Edge* currentEdge = CurrentEdge(node);
				PrintGraph(node, currentEdge);
				if (currentEdge == NULL)
					Relable(node);
				else
					Push(node);
			}
			FinishProcessingNode(node);
		}
	}
	PrintGraph(0, 0);
	uint32_t flow = this->target->GetTag<NodeTag*>()->excess;
	CleanUp();
	return flow;
}

//O(1)
void ModifiedKRT::AddEdge(Edge* edge)
{
	for (int i = 0; i < 2; i++)
	{
		EdgeTag* edgeTag = edge->GetTag<EdgeTag*>();
		edgeTag->isAdded = true;
		Node* source = edge->LinkedEdge->TargetNode;
		Node* target = edge->TargetNode;
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		NodeTag* targetTag = target->GetTag<NodeTag*>();

		sourceTag->hiddenCapacities -= edge->Capasity;
		UpdateVisibleExcess(source);
		if (sourceTag->visibleExcess > 0)
			readyNodes.AddFirst(source);
		edge = edge->LinkedEdge;
	}
}

//O(1) except for UpdateRl and DesignateEdge
void ModifiedKRT::Push(Edge* edge, uint32_t flow)
{
	if (flow == 0)
		return;
	EdgeTag* edgeTag = edge->GetTag<EdgeTag*>();
	Node* source = edge->LinkedEdge->TargetNode;
	Node* target = edge->TargetNode;
	NodeTag* sourceTag = source->GetTag<NodeTag*>();
	NodeTag* targetTag = target->GetTag<NodeTag*>();
	edge->Capasity -= flow;
	edge->LinkedEdge->Capasity += flow;
	sourceTag->excess -= flow;
	targetTag->excess += flow;

	if (!edgeTag->isAdded)
	{
		sourceTag->hiddenCapacities -= flow;
		targetTag->hiddenCapacities += flow;
	}


	UpdateVisibleExcess(source);
	UpdateVisibleExcess(target);
	if (targetTag->visibleExcess > 0 && targetTag->visibleExcess <= flow)
		readyNodes.AddFirst(target);
	if (edge->LinkedEdge->Capasity == flow)
	{
		EdgeTag* linkedEdgeTag = edge->LinkedEdge->GetTag<EdgeTag*>();
		linkedEdgeTag->sourceElm = targetTag->source.levelInegibleEdges.AddFirst(linkedEdgeTag);
		
	}
	if (edge->Capasity == 0)
	{
		sourceTag->source.activeEdges.Remove(edgeTag->sourceElm);
		sourceTag->source.activeEdgesByErl[targetTag->target.erl].Remove(edgeTag->sourceErlElm);
		targetTag->target.activeEdges.Remove(edgeTag->targetElm);
		edgeTag->sourceElm = 0;
		edgeTag->sourceErlElm = 0;
		edgeTag->targetElm = 0;
		if (edgeTag == sourceTag->source.currentEdge)
		{
			if (sourceTag->source.activeEdges.GetCount() + 1 >= l)
			{
				targetTag->target.designatedEdges--;
				UpdateRl(target);
			}
			DesignateEdge(source);
		}
	}
}

//O(length of path)
void ModifiedKRT::Push(Node* node)
{
	Edge* edge = CurrentEdge(node);
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	uint32_t flow = nodeTag->visibleExcess;

	while (edge != NULL)
	{
		if (edge->Capasity < flow)
		{
			Push(edge, edge->Capasity);
			break;
		}
		Push(edge, flow);
		edge = CurrentEdge(edge->TargetNode);
	}
}

void ModifiedKRT::Relable(Node* node)
{
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	//Remove all active edges to this node, if the levels are now equal
	LinkedList<EdgeTag*>::Node* linkedListNode = nodeTag->target.activeEdges.GetHead();
	while (linkedListNode != NULL)
	{
		EdgeTag* edgeTag = linkedListNode->GetInstance();
		linkedListNode = linkedListNode->GetNext();
		Node* source = edgeTag->edge->LinkedEdge->TargetNode;
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		if (sourceTag->lable > nodeTag->lable + 1)
		{
			if (nodeTag->target.erl > 0)
				sourceTag->source.activeEdgesByErl[0].MoveFirst(sourceTag->source.activeEdgesByErl + nodeTag->target.erl, edgeTag->sourceErlElm);
			continue;
		}
		sourceTag->source.levelInegibleEdges.MoveFirst(&sourceTag->source.activeEdges, edgeTag->sourceElm);
		sourceTag->source.activeEdgesByErl[nodeTag->target.erl].Remove(edgeTag->sourceErlElm);
		edgeTag->sourceErlElm = 0;
		if (edgeTag == sourceTag->source.currentEdge)
			DesignateEdge(source);
		nodeTag->target.activeEdges.Remove(edgeTag->targetElm);
	}
	nodeTag->target.designatedEdges = 0;
	nodeTag->target.erl = 0;
	nodeTag->lable++;
	//Add edges from this node that was previously inegible due to level
	linkedListNode = nodeTag->source.levelInegibleEdges.GetHead();
	while (linkedListNode != NULL)
	{
		EdgeTag* edgeTag = linkedListNode->GetInstance();
		linkedListNode = linkedListNode->GetNext();
		Node* target = edgeTag->edge->TargetNode;
		NodeTag* targetTag = target->GetTag<NodeTag*>();
		if (nodeTag->lable > targetTag->lable)
		{
			nodeTag->source.activeEdges.MoveFirst(&nodeTag->source.levelInegibleEdges, edgeTag->sourceElm);
			edgeTag->sourceErlElm = nodeTag->source.activeEdgesByErl[targetTag->target.erl].AddFirst(edgeTag);
			edgeTag->targetElm = targetTag->target.activeEdges.AddFirst(edgeTag);
		}
	}
	DesignateEdge(node);
}

Edge* ModifiedKRT::CurrentEdge(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (tag->source.currentEdge == NULL)
		return NULL;
	return tag->source.currentEdge->edge;
}

void ModifiedKRT::UpdateVisibleExcess(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (tag->excess < tag->hiddenCapacities)
		tag->visibleExcess = 0;
	else
		tag->visibleExcess = tag->excess - tag->hiddenCapacities;
}

void ModifiedKRT::DesignateEdge(Node* node)
{
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	if (nodeTag->source.activeEdges.GetCount() <= l)
	{
		if (nodeTag->source.activeEdges.GetCount() == 0)
			nodeTag->source.currentEdge = NULL;
		else
			nodeTag->source.currentEdge = nodeTag->source.activeEdges.GetHead()->GetInstance();
		return;
	}

	for (int i = 0; i <= t; i++)
	{
		if (!nodeTag->source.activeEdgesByErl[i].IsEmpty())
		{
			nodeTag->source.currentEdge = nodeTag->source.activeEdgesByErl[i].GetHead()->GetInstance();
			break;
		}
	}
	
	nodeTag->source.currentEdge->edge->TargetNode->GetTag<NodeTag*>()->target.designatedEdges++;
	UpdateRl(nodeTag->source.currentEdge->edge->TargetNode);
}

void ModifiedKRT::UpdateRl(Node* node)
{
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	nodeTag->target.rl = 0;
	float ratio = nodeTag->target.designatedEdges/(float)node->EdgeCount;
	float r = r0;
	while (ratio > r)
	{
		r *= 2;
		nodeTag->target.rl++;
	}
	if (nodeTag->target.rl > t)
		nodeTag->target.rl = t;
	if (nodeTag->target.rl > nodeTag->target.erl ||
		nodeTag->target.rl + 2 < nodeTag->target.erl)
		UpdateErl(node);
}

void ModifiedKRT::UpdateErl(Node* node)
{
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	if (nodeTag->target.rl == nodeTag->target.erl)
		return;
	
	LinkedList<EdgeTag*>::Node* linkedListNode = nodeTag->target.activeEdges.GetHead();
	while (linkedListNode != NULL)
	{
		EdgeTag* edgeTag = linkedListNode->GetInstance();
		linkedListNode = linkedListNode->GetNext();
		Node* source = edgeTag->edge->LinkedEdge->TargetNode;
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		sourceTag->source.activeEdgesByErl[nodeTag->target.rl].MoveFirst(sourceTag->source.activeEdgesByErl + nodeTag->target.erl, edgeTag->sourceErlElm);
	}
	nodeTag->target.erl = nodeTag->target.rl;
	if (nodeTag->target.erl == t && node != this->source && node != this->target)
		Reset();
}

void ModifiedKRT::Reset()
{
	if (t <= 2)
		return;
	if (resetting)
		return;
	resetting = true;
	LinkedList<Node*>* targetsByRl = new LinkedList<Node*>[t + 1];
	LinkedList<Node*>* sourcesByRl = new LinkedList<Node*>[t + 1];
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		NodeTag* nodeTag = node->GetTag<NodeTag*>();
		targetsByRl[nodeTag->target.rl].AddFirst(node);
		if (nodeTag->source.activeEdges.GetCount() < l)
			continue;
		Node* target = nodeTag->source.currentEdge->edge->TargetNode;
		NodeTag* targetTag = target->GetTag<NodeTag*>();
		sourcesByRl[targetTag->target.rl].AddFirst(node);
	}
	uint32_t* vCounts = new uint32_t[t + 1];
	uint32_t* uCounts = new uint32_t[t + 1];
	float r = r0;
	for (int i = t; i >= 0; i--)
	{
		vCounts[i] = targetsByRl[i].GetCount();
		uCounts[i] = targetsByRl[i].GetCount();
		if (i < t)
		{
			vCounts[i] += vCounts[i + 1];
			uCounts[i] += uCounts[i + 1];
			r *= 2;
		}
	}
	//Find k
	uint32_t k = t;
	while (k > 3 && uCounts[k - 2] >= r*l*uCounts[k]/2)
	{
		k -= 2;
		r /= 4;
	}
	//Update target ERL
	LinkedList<Node*>::Node* linkedListElement = targetsByRl[k - 2].GetHead();
	while (linkedListElement != NULL)
	{
		UpdateErl(linkedListElement->GetInstance());
		linkedListElement = linkedListElement->GetNext();
	}
	//Undesignate edges
	for (int i = k - 2; i <= t; i++)
	{
		LinkedList<Node*>::Node* linkedListElement = sourcesByRl[i].GetHead();
		while (linkedListElement != NULL)
		{
			Node* node = linkedListElement->GetInstance();
			NodeTag* nodeTag = node->GetTag<NodeTag*>();
			Node* target = nodeTag->source.currentEdge->edge->TargetNode;
			NodeTag* targetTag = target->GetTag<NodeTag*>();
			nodeTag->source.currentEdge = NULL;
			targetTag->target.designatedEdges--;
			linkedListElement = linkedListElement->GetNext();
		}
	}
	//Update target ERL
	for (int i = k - 2; i <= t; i++)
	{
		LinkedList<Node*>::Node* linkedListElement = targetsByRl[i].GetHead();
		while (linkedListElement != NULL)
		{
			UpdateRl(linkedListElement->GetInstance());
			linkedListElement = linkedListElement->GetNext();
		}
	}
	//Designate edges
	for (int i = k - 2; i <= t; i++)
	{
		LinkedList<Node*>::Node* linkedListElement = sourcesByRl[i].GetHead();
		while (linkedListElement != NULL)
		{
			DesignateEdge(linkedListElement->GetInstance());
			linkedListElement = linkedListElement->GetNext();
		}
	}

	delete[] targetsByRl;
	delete[] sourcesByRl;
	delete[] vCounts;
	delete[] uCounts;
	resetting = false;
}

void ModifiedKRTGlobalRename::Initialize()
{
	performingGlobalRelabel = true;
	ModifiedKRT::Initialize();
	performingGlobalRelabel = false;
	globalRelabelQueue = new Queue<Node*>(graph->NodeCount);
	GlobalRelabel();
}

void ModifiedKRTGlobalRename::CleanUp()
{
	IsCleaning = true;
	ModifiedKRT::CleanUp();
	delete globalRelabelQueue;
}

void ModifiedKRTGlobalRename::Relable(Node* node)
{
	ModifiedKRT::Relable(node);
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (CurrentEdge(node) == 0)
		GlobalRelabel();
}

void ModifiedKRTGlobalRename::GlobalRelabel()
{
	if (performingGlobalRelabel)
		return;
	performingGlobalRelabel = true;

	globalRelabelQueue->Enqueue(target);

	NodeTag* tag = target->GetTag<NodeTag*>();
	uint32_t currentIteration = tag->globalRelabelIteration;

	tag->globalRelabelIteration++;

	//Labels from target
	while (!globalRelabelQueue->IsEmpty())
	{
		Node* node = globalRelabelQueue->Dequeue();
		tag = node->GetTag<NodeTag*>();
		tag->globalRelabelIteration++;
		
		if (node == source)
			continue;

		uint32_t requiredLabel = 2*graph->NodeCount;
		if (node == target)
			requiredLabel = 0;
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			Node* neighbor = edge->TargetNode;
			NodeTag* neighborTag = neighbor->GetTag<NodeTag*>();

			if (neighborTag->globalRelabelIteration == currentIteration && edge->LinkedEdge->Capasity > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && edge->Capasity > 0)
			{
				if (requiredLabel > neighborTag->lable + 1)
					requiredLabel = neighborTag->lable + 1;
			}
		}
		
		if (requiredLabel > tag->lable)
			RelableTo(node, requiredLabel);
	}

	globalRelabelQueue->Enqueue(source);
	tag = source->GetTag<NodeTag*>();
	tag->globalRelabelIteration = currentIteration + 1;
	
	//Labels from source
	while (!globalRelabelQueue->IsEmpty())
	{
		Node* node = globalRelabelQueue->Dequeue();
		tag = node->GetTag<NodeTag*>();
		tag->globalRelabelIteration++;
		
		if (node == target)
			continue;

		uint32_t requiredLabel = 2*graph->NodeCount;
		if (node == source)
			requiredLabel = graph->NodeCount;
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			Node* neighbor = edge->TargetNode;
			NodeTag* neighborTag = neighbor->GetTag<NodeTag*>();

			if (neighborTag->globalRelabelIteration == currentIteration && edge->LinkedEdge->Capasity > 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && edge->Capasity > 0)
			{
				if (requiredLabel > neighborTag->lable + 1)
					requiredLabel = neighborTag->lable + 1;
			}
		}
		
		
		if (requiredLabel > tag->lable)
			RelableTo(node, requiredLabel);
	}
	
	
	performingGlobalRelabel = false;
}

void ModifiedKRTGlobalRename::RelableTo(Node* node, uint32_t targetLabel)
{	
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	//Remove all active edges to this node, if the levels are now equal
	LinkedList<EdgeTag*>::Node* linkedListNode = nodeTag->target.activeEdges.GetHead();
	while (linkedListNode != NULL)
	{
		EdgeTag* edgeTag = linkedListNode->GetInstance();
		linkedListNode = linkedListNode->GetNext();
		Node* source = edgeTag->edge->LinkedEdge->TargetNode;
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		if (sourceTag->lable > targetLabel)
		{
			if (nodeTag->target.erl > 0)
				sourceTag->source.activeEdgesByErl[0].MoveFirst(sourceTag->source.activeEdgesByErl + nodeTag->target.erl, edgeTag->sourceErlElm);
			continue;
		}
		sourceTag->source.levelInegibleEdges.MoveFirst(&sourceTag->source.activeEdges, edgeTag->sourceElm);
		sourceTag->source.activeEdgesByErl[nodeTag->target.erl].Remove(edgeTag->sourceErlElm);
		edgeTag->sourceErlElm = 0;
		if (edgeTag == sourceTag->source.currentEdge)
			DesignateEdge(source);
		nodeTag->target.activeEdges.Remove(edgeTag->targetElm);
	}
	nodeTag->target.designatedEdges = 0;
	nodeTag->target.erl = 0;
	nodeTag->lable = targetLabel;
	//Add edges from this node that was previously inegible due to level
	linkedListNode = nodeTag->source.levelInegibleEdges.GetHead();
	while (linkedListNode != NULL)
	{
		EdgeTag* edgeTag = linkedListNode->GetInstance();
		linkedListNode = linkedListNode->GetNext();
		Node* target = edgeTag->edge->TargetNode;
		NodeTag* targetTag = target->GetTag<NodeTag*>();
		if (nodeTag->lable > targetTag->lable)
		{
			nodeTag->source.activeEdges.MoveFirst(&nodeTag->source.levelInegibleEdges, edgeTag->sourceElm);
			edgeTag->sourceErlElm = nodeTag->source.activeEdgesByErl[targetTag->target.erl].AddFirst(edgeTag);
			edgeTag->targetElm = targetTag->target.activeEdges.AddFirst(edgeTag);
		}
	}
	DesignateEdge(node);
}



void ModifiedKRTRareGlobalRename::Initialize()
{
	passEndNode = 0;
	excessLastPass = 0;
	excessLastRelabel = 0;
	ModifiedKRTGlobalRename::Initialize();
}

void ModifiedKRTRareGlobalRename::Relable(Node* node)
{
	ModifiedKRT::Relable(node);
}

void ModifiedKRTRareGlobalRename::GlobalRelabel()
{
	ModifiedKRTGlobalRename::GlobalRelabel();
	NodeTag* tag = target->GetTag<NodeTag*>();
	if (readyNodes.GetCount() == 0)
		passEndNode = 0;
	else
		passEndNode = readyNodes.GetTail()->GetInstance();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void ModifiedKRTRareGlobalRename::FinishProcessingNode(Node* node)
{
	if (passEndNode != 0 && node != passEndNode && passEndNode != source && passEndNode != target)
		return;

	NodeTag* tag = target->GetTag<NodeTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
	{
		if (readyNodes.GetCount() == 0)
			passEndNode = 0;
		else
			passEndNode = readyNodes.GetTail()->GetInstance();
		excessLastPass = tag->excess;
	}
}


void ModifiedKRTRareGlobalRename2::Initialize()
{
	nodesProcessLimit = (uint32_t)(1.5834*pow(graph->NodeCount, 0.7578));
	ModifiedKRTRareGlobalRename::Initialize();
}

void ModifiedKRTRareGlobalRename2::SetCheckLimit(uint32_t limit)
{
	nodesProcessLimit = limit;
}

void ModifiedKRTRareGlobalRename2::GlobalRelabel()
{
	ModifiedKRTGlobalRename::GlobalRelabel();
	NodeTag* tag = target->GetTag<NodeTag*>();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void ModifiedKRTRareGlobalRename2::FinishProcessingNode(Node* node)
{	
	nodesProcessed++;
	if (nodesProcessed < nodesProcessLimit)
		return;
	nodesProcessed = 0;
	NodeTag* tag = target->GetTag<NodeTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
		excessLastPass = tag->excess;
}


#include <fstream>
#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */

#include <direct.h>
#include <sstream>
#define GetCurrentDir _getcwd

using namespace std;

int graphNr = 0;
void ModifiedKRT::PrintGraph(Node* activeNode, Edge* activeEdge)
{
	return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");
	writer.open(tempFile);
	writer << "digraph G {";
	for (uint32_t i = 0; i < graph->NodeCount; i++)
	{
		if (i != 0)
			writer << ';';
		NodeTag* tag = graph->Nodes[i].GetTag<NodeTag*>();
		writer << "\"Node: " << i + 1 << "\n Label:" << tag->lable << "\n Excess:" << tag->visibleExcess << "/" << tag->excess << "\"";
		if (activeNode == graph->Nodes + i)
			writer << "[color=green]";

	}
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			//if (edge->Capasity > 0)
			{
				NodeTag* tag = node->GetTag<NodeTag*>();
				writer << ";\"Node: " << node - graph->Nodes + 1  << "\n Label:" << tag->lable << "\n Excess:" << tag->visibleExcess << "/" << tag->excess;
				tag = edge->TargetNode->GetTag<NodeTag*>();
				writer << "\"->\"Node: " << edge->TargetNode - graph->Nodes + 1  << "\n Label:" << tag->lable << "\n Excess:" << tag->visibleExcess << "/" << tag->excess;
				writer << "\"[label=" << edge->Capasity;
				if (edge == CurrentEdge(node))
					writer << ", color=blue";
				else if (!edge->GetTag<EdgeTag*>()->isAdded)
					writer << ", color=red";
				writer  << "]";
			}
		}
	}
	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << "\"C:\\Program Files (x86)\\Graphviz2.30\\bin\\dot.exe\" -Tjpg " << tempFile;
	ss << " -o D:\\Users\\Beier\\Documents\\MasterThesis\\MaxFlow\\Graphs\\MKRTGraph" << graphNr++ << ".jpg";

	int r = system(ss.str().c_str());
}