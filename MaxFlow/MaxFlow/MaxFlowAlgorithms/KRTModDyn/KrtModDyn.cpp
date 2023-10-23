#include "KrtModDyn.h"

int KrtModDyn::edgeComparator(const void * a, const void * b)
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


int KrtModDyn::edgepComparator(const void * a, const void * b)
{
	Edge** edge1 = (Edge**)a;
	Edge** edge2 = (Edge**)b;
	return edgeComparator(*edge1, *edge2);
}

//O(nm)
void KrtModDyn::Initialize()
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

	
	forest = new DynamicForest<Node>(graph->Nodes, graph->NodeCount);

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
			if (e->Capasity > 0)
				edgeTag->sourceElm = nodeTag->source.levelInegibleEdges.AddFirst(edgeTag);
			else
				edgeTag->sourceElm = 0;
			edgeTag->sourceErlElm = 0;
			edgeTag->targetElm = 0;
			edgeTag->isAdded = false;
			nodeTag->hiddenCapacities += e->Capasity;
			edgeTag++;
		}
	}
	//Push from source
	for (Edge* e = source->Edges; e < source->Edges + source->EdgeCount; e++)
	{
		AddEdge(e);
	}
	Relable(source);
	for (Edge* e = source->Edges; e < source->Edges + source->EdgeCount; e++)
	{
		Saturate(e);
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
void KrtModDyn::CleanUp()
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
	delete forest;
	LinkedListNodeManager::ClearCache();
}

uint32_t KrtModDyn::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	this->graph = graph;
	this->source = source;
	this->target = target;
	Initialize();
	for (int i = 0; i < graph->EdgeCount/2 - source->EdgeCount; i++)
	{

		Edge* edge = edgesToInsert[i];
		AddEdge(edge);

		
		Node* source = edge->LinkedEdge->TargetNode;
		Node* target = edge->TargetNode;
		
		NodeTag* sourceTag = source->GetTag<NodeTag*>();
		NodeTag* targetTag = target->GetTag<NodeTag*>();
		
		if (targetTag->lable < sourceTag->lable && GetCapacity(edge) <= sourceTag->visibleExcess)
			Saturate(edge);
		else if (sourceTag->lable < targetTag->lable && GetCapacity(edge->LinkedEdge) <= targetTag->visibleExcess)
			Saturate(edge->LinkedEdge);

		while (!readyNodes.IsEmpty())
		{
			Node* node = readyNodes.RemoveFirst();
			NodeTag* nodeTag = node->GetTag<NodeTag*>();
			if (node == this->source || node == this->target)
				continue;
			while (nodeTag->visibleExcess > 0)
			{
				Edge* currentEdge = CurrentEdge(node);
				if (currentEdge == NULL)
					Relable(node);
				else
					TreePush(node);
			}
			
			FinishProcessingNode(node);
			
		}
	}
	uint32_t flow = this->target->GetTag<NodeTag*>()->excess;

	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		NodeTag* tag = node->GetTag<NodeTag*>();
		
		Edge* edge = CurrentEdge(node);
		if (edge == NULL)
			continue;
		uint32_t cost = forest->GetCost(node);
		uint32_t diff = edge->Capasity - cost;
		forest->Cut(node);
		if (diff == 0)
			continue;
		edge->Capasity = cost;
		edge->LinkedEdge->Capasity += diff;
	}

	CleanUp();
	return flow;
}

//O(1)
void KrtModDyn::AddEdge(Edge* edge)
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
		edge = edge->LinkedEdge;
	}
}

//O(1) except for UpdateRl and DesignateEdge
void KrtModDyn::Saturate(Edge* edge)
{
	EdgeTag* edgeTag = edge->GetTag<EdgeTag*>();
	Node* source = edge->LinkedEdge->TargetNode;
	Node* target = edge->TargetNode;
	NodeTag* sourceTag = source->GetTag<NodeTag*>();
	NodeTag* targetTag = edge->TargetNode->GetTag<NodeTag*>();
	if (forest->GetLink(source) == edge->TargetNode)
	{			
		uint32_t cost = forest->GetCost(source);
		sourceTag->excess -= cost;
		UpdateVisibleExcess(source);
		targetTag->excess += cost;
		UpdateVisibleExcess(edge->TargetNode);
		forest->SetCost(source, 0);
	}
	else
	{

		if (edge->LinkedEdge->Capasity == 0 && edge->Capasity > 0)
		{
			EdgeTag* linkedEdgeTag = edge->LinkedEdge->GetTag<EdgeTag*>();
			linkedEdgeTag->sourceElm = targetTag->source.levelInegibleEdges.AddFirst(linkedEdgeTag);		
		}

		sourceTag->excess -= edge->Capasity;
		targetTag->excess += edge->Capasity;

		//If not added
		if (!edge->GetTag<EdgeTag*>()->isAdded)
		{
			sourceTag->hiddenCapacities -= edge->Capasity;
			targetTag->hiddenCapacities += edge->Capasity;
		}
		UpdateVisibleExcess(source);
		UpdateVisibleExcess(edge->TargetNode);

		edge->LinkedEdge->Capasity += edge->Capasity;
		edge->Capasity = 0;

	}
	if (edgeTag->sourceElm != 0)
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
void KrtModDyn::TreePush(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	Edge* edge /*= CurrentEdge(node);
	if (!edge->GetTag<EdgeTag*>()->isAdded)
		tag->visibleExcess = edge->Capasity*/;

	Node* bounding = forest->GetBoundingNode(node, tag->visibleExcess);
	if (bounding != NULL)
	{
		

		NodeTag* tag2 = bounding->GetTag<NodeTag*>();
		edge = CurrentEdge(bounding);
		if (node != bounding)
			tag2->excess += tag->visibleExcess;
		Saturate(edge);
	}
	else
	{
		Node* target = forest->GetRoot(node);
		NodeTag* tag2 = target->GetTag<NodeTag*>();
		tag2->excess += tag->visibleExcess;
		UpdateVisibleExcess(target);
	}
	if (node != bounding)
	{
		if (bounding != NULL && forest->IsLinked(bounding))
		{
			Node* target = forest->GetLink(bounding);
			forest->Cut(bounding);
			forest->AddCost(node, -tag->visibleExcess);
			forest->Link(bounding, target);
		}
		else
			forest->AddCost(node, -tag->visibleExcess);
		tag->excess -= tag->visibleExcess;
		UpdateVisibleExcess(node);
	}
}

void KrtModDyn::Relable(Node* node)
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
		nodeTag->target.activeEdges.Remove(edgeTag->targetElm);
		edgeTag->targetElm = 0;
		if (edgeTag == sourceTag->source.currentEdge)
			DesignateEdge(source);
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

Edge* KrtModDyn::CurrentEdge(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (tag->source.currentEdge == NULL)
		return NULL;
	return tag->source.currentEdge->edge;
}

void KrtModDyn::UpdateVisibleExcess(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	bool hadVisibleExcess = tag->visibleExcess > 0;
	if (tag->excess < tag->hiddenCapacities)
		tag->visibleExcess = 0;
	else
		tag->visibleExcess = tag->excess - tag->hiddenCapacities;
	if (!hadVisibleExcess && tag->visibleExcess > 0)
	{
		readyNodes.AddLast(node);
	}
}

void KrtModDyn::DesignateEdge(Node* node)
{
	Cut(node);
	NodeTag* nodeTag = node->GetTag<NodeTag*>();
	if (nodeTag->source.activeEdges.GetCount() <= l)
	{
		if (nodeTag->source.activeEdges.GetCount() == 0)
			nodeTag->source.currentEdge = NULL;
		else
		{
			nodeTag->source.currentEdge = nodeTag->source.activeEdges.GetHead()->GetInstance();
			Link(node);
		}
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
	Link(node);
	nodeTag->source.currentEdge->edge->TargetNode->GetTag<NodeTag*>()->target.designatedEdges++;
	UpdateRl(nodeTag->source.currentEdge->edge->TargetNode);
}

void KrtModDyn::UpdateRl(Node* node)
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

void KrtModDyn::UpdateErl(Node* node)
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

void KrtModDyn::Reset()
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
			Cut(node);
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


void KrtModDyn::Cut(Node* node)
{
	if (forest->IsLinked(node))
	{
		Edge* currentEdge = CurrentEdge(node);
		uint32_t cap = forest->GetCost(node);
		uint32_t diff = currentEdge->Capasity - cap;
		NodeTag* targetTag = currentEdge->TargetNode->GetTag<NodeTag*>();
		
		//If not added
		if (!currentEdge->GetTag<EdgeTag*>()->isAdded)
		{
			node->GetTag<NodeTag*>()->hiddenCapacities -= diff;
			targetTag->hiddenCapacities += diff;
			UpdateVisibleExcess(node);
			UpdateVisibleExcess(currentEdge->TargetNode);
		}

		
		if (currentEdge->LinkedEdge->Capasity == 0 && diff > 0)
		{
			EdgeTag* linkedEdgeTag = currentEdge->LinkedEdge->GetTag<EdgeTag*>();
			linkedEdgeTag->sourceElm = targetTag->source.levelInegibleEdges.AddFirst(linkedEdgeTag);		
		}

		currentEdge->LinkedEdge->Capasity += diff;
		currentEdge->Capasity = cap;
		forest->Cut(node);
	}
}

void KrtModDyn::Link(Node* node)
{
	Edge* currentEdge = CurrentEdge(node);
	if (forest->IsLinked(node))
		throw "ARARARS";
	if (currentEdge != NULL)
	{
		forest->SetCost(node, currentEdge->Capasity);
		forest->Link(node, currentEdge->TargetNode);
	}
}


uint32_t KrtModDyn::GetCapacity(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	uint32_t capacity = edge->Capasity;
	if (edge->TargetNode == forest->GetLink(node))
		capacity = forest->GetCost(node);
	if (node == forest->GetLink(edge->TargetNode))
		capacity += edge->LinkedEdge->Capasity - forest->GetCost(edge->TargetNode);
	return capacity;
}


void KrtModDynGlobalRename::Initialize()
{
	performingGlobalRelabel = true;
	KrtModDyn::Initialize();
	performingGlobalRelabel = false;
	globalRelabelQueue = new Queue<Node*>(graph->NodeCount);
	GlobalRelabel();
}

void KrtModDynGlobalRename::CleanUp()
{
	IsCleaning = true;
	KrtModDyn::CleanUp();
	delete globalRelabelQueue;
}

void KrtModDynGlobalRename::Relable(Node* node)
{
	KrtModDyn::Relable(node);
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (CurrentEdge(node) == 0)
		GlobalRelabel();
}

void KrtModDynGlobalRename::GlobalRelabel()
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

			if (neighborTag->globalRelabelIteration == currentIteration && GetCapacity(edge->LinkedEdge)> 0)
			{
				globalRelabelQueue->Enqueue(neighbor);
				neighborTag->globalRelabelIteration++;
			}
			else if (neighborTag->globalRelabelIteration == currentIteration + 2 && GetCapacity(edge) > 0)
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

void KrtModDynGlobalRename::RelableTo(Node* node, uint32_t targetLabel)
{	
	bool checkConsistency = false;
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
		nodeTag->target.activeEdges.Remove(edgeTag->targetElm);
		edgeTag->targetElm = 0;
		if (edgeTag == sourceTag->source.currentEdge)
			DesignateEdge(source);
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



void KrtModDynRareGlobalRename::Initialize()
{
	passEndNode = 0;
	excessLastPass = 0;
	excessLastRelabel = 0;
	KrtModDynGlobalRename::Initialize();
}

void KrtModDynRareGlobalRename::Relable(Node* node)
{
	KrtModDyn::Relable(node);
}

void KrtModDynRareGlobalRename::GlobalRelabel()
{
	KrtModDynGlobalRename::GlobalRelabel();
	NodeTag* tag = target->GetTag<NodeTag*>();
	if (readyNodes.GetCount() == 0)
		passEndNode = 0;
	else
		passEndNode = readyNodes.GetTail()->GetInstance();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void KrtModDynRareGlobalRename::FinishProcessingNode(Node* node)
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


void KrtModDynRareGlobalRename2::Initialize()
{
	nodesProcessLimit = (uint32_t)(1.6078*pow(graph->NodeCount, 0.6509));
	KrtModDynRareGlobalRename::Initialize();
}

void KrtModDynRareGlobalRename2::SetCheckLimit(uint32_t limit)
{
	nodesProcessLimit = limit;
}

void KrtModDynRareGlobalRename2::GlobalRelabel()
{
	KrtModDynGlobalRename::GlobalRelabel();
	NodeTag* tag = target->GetTag<NodeTag*>();
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void KrtModDynRareGlobalRename2::FinishProcessingNode(Node* node)
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

/*		
#include <fstream>
#include <iostream>

#include <stdio.h> 

#include <direct.h>
#include <sstream>
#define GetCurrentDir _getcwd

using namespace std;

void KrtModDyn::PrintGraph(Node* activeNode, Edge* activeEdge)
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
				writer << "\"[label=" << GetCapacity(edge);
				if (edge == activeEdge || edge->LinkedEdge == activeEdge)
					writer << ", color=green";
				else if (edge == CurrentEdge(node))
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
	ss << " -o D:\\Users\\Beier\\Documents\\MasterThesis\\MaxFlow\\Graphs\\MKRTDynGraph" << graphNr++ << ".jpg";

	int r = system(ss.str().c_str());
}
*//*
void KrtModDyn::AssertConsistent()
{
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		NodeTag* nodeTag = node->GetTag<NodeTag*>();
		LinkedList<EdgeTag*>::Node* linkedListNode = nodeTag->target.activeEdges.GetHead();
		while (linkedListNode != NULL)
		{
			LinkedList<EdgeTag*>::Node* currNode = linkedListNode;
			EdgeTag* edgeTag = linkedListNode->GetInstance();
			linkedListNode = linkedListNode->GetNext();



			Node* source = edgeTag->edge->LinkedEdge->TargetNode;
			NodeTag* sourceTag = source->GetTag<NodeTag*>();
			
			if (!sourceTag->source.activeEdgesByErl[nodeTag->target.erl].Contains(edgeTag->sourceErlElm))

			//if (edgeTag->targetElm != currNode)
				printf("Error");
		}
		linkedListNode = nodeTag->source.levelInegibleEdges.GetHead();
		while (linkedListNode != NULL)
		{
			LinkedList<EdgeTag*>::Node* currNode = linkedListNode;
			EdgeTag* edgeTag = linkedListNode->GetInstance();
			linkedListNode = linkedListNode->GetNext();
			Node* source = edgeTag->edge->LinkedEdge->TargetNode;
			NodeTag* sourceTag = source->GetTag<NodeTag*>();
			
			if (edgeTag->sourceElm != currNode)
				printf("Error");
			if (edgeTag->targetElm != 0)
				printf("Error");
			if (edgeTag->edge->Capasity == 0)
				printf("Error");
		}
		
		linkedListNode = nodeTag->source.activeEdges.GetHead();
		while (linkedListNode != NULL)
		{
			LinkedList<EdgeTag*>::Node* currNode = linkedListNode;
			EdgeTag* edgeTag = linkedListNode->GetInstance();
			linkedListNode = linkedListNode->GetNext();
			Node* source = edgeTag->edge->LinkedEdge->TargetNode;
			NodeTag* sourceTag = source->GetTag<NodeTag*>();
			
			if (edgeTag->sourceElm != currNode)
				printf("Error");
		}
	}

}*/