
#include "KingRao1992.h"


uint32_t KR1992::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	this->graph = graph;
	this->source = source;
	this->target = target;
	Initialize();
	for (int i = 0; i < edgesToAddLength; i++)
	{
		Edge* edge = edgesToAdd[i];
		AddEdge(edge);
		AddEdge(edge->LinkedEdge);

		Node* n1 = edge->LinkedEdge->TargetNode;
		Node* n2 = edge->TargetNode;
		NodeTag* n1Tag = n1->GetTag<NodeTag*>();
		NodeTag* n2Tag = n2->GetTag<NodeTag*>();
		
		
		if (n1Tag->label > n2Tag->label && edge->Capasity <= n1Tag->visibleExcess && edge->Capasity > 0)
			Saturate(edge);
		else if (n2Tag->label > n1Tag->label && edge->LinkedEdge->Capasity <= n2Tag->visibleExcess && edge->LinkedEdge->Capasity > 0)
			Saturate(edge->LinkedEdge);


		while (!activeNodes.IsEmpty())
		{
			Node* node = activeNodes.RemoveFirst();
			
			NodeTag* tag = node->GetTag<NodeTag*>();
			if (node == source || node == target || tag->visibleExcess == 0)
				continue;
			while (tag->visibleExcess > 0)
			{
				Edge* e = game->CurrentEdge(node, tag->label);

				if (e == NULL)
					Relable(node);
				else
					TreePush(node);
			}
			FinishProcessingNode(node);
		}
	}
	
	

	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		NodeTag* tag = node->GetTag<NodeTag*>();
		
		Edge* edge = game->CurrentEdge(node, node->GetTag<NodeTag*>()->label);
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

	uint32_t maxFlow = target->GetTag<NodeTag*>()->excess;
	
	CleanUp();

	return maxFlow;
}

void KR1992::Initialize()
{
	graphNr = 0;

	//Initialize node tags
	nodeTags = new NodeTag[graph->NodeCount];
	NodeTag* tag = nodeTags;
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++, tag++)
	{
		node->Tag = tag;
		tag->excess = 0;
		tag->label = 0;
		tag->visibleExcess = 0;
		tag->hiddenCapacities = 0;
		tag->globalRelabelIteration = 0;
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			tag->hiddenCapacities += edge->Capasity;
			if (node == this->source)
				tag->excess += edge->Capasity;
			edge->Tag = 0;
		}
	}

	//Set up tree
	forest = new DynamicForest<Node>(graph->Nodes, graph->NodeCount);
	

	//Set up game
	game = new Game(this);


	//Push from source
	Relable(source);
	for (Edge* edge = source->Edges; edge < source->Edges + source->EdgeCount; edge++)
	{
		AddEdge(edge);
		AddEdge(edge->LinkedEdge);
		Saturate(edge);
	}
	tag = source->GetTag<NodeTag*>();
	while (tag->label < graph->NodeCount)
	{
		Relable(source);
	}

	//Sort edges acording to ucap
	edgesToAddLength = graph->EdgeCount/2 - source->EdgeCount;
	edgesToAdd = new Edge*[graph->EdgeCount/2 - source->EdgeCount];
	Edge** nextEdgePos = edgesToAdd;
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
	qsort(edgesToAdd, graph->EdgeCount/2 - source->EdgeCount, sizeof(Edge*), edgepComparator);
}

void KR1992GlobalRename::Initialize()
{
	performingGlobalRelabel = true;
	KR1992::Initialize();
	performingGlobalRelabel = false;
	globalRelabelQueue = new Queue<Node*>(graph->NodeCount);
	GlobalRelabel();
}


void KR1992::CleanUp()
{
	IsCleaning = true;
	delete game;
	delete forest;
	delete[] nodeTags;
	delete[] edgesToAdd;
	LinkedListNodeManager::ClearCache();
}

void KR1992GlobalRename::CleanUp()
{
	IsCleaning = true;
	KR1992::CleanUp();
	delete globalRelabelQueue;
}

int KR1992::edgepComparator(const void * a, const void * b)
{
	Edge* edge1 = *(Edge**)a;	
	Edge* edge2 = *(Edge**)b;

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

void KR1992::AddEdge(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	NodeTag* tag = node->GetTag<NodeTag*>();
	tag->hiddenCapacities -= edge->Capasity;
	UpdateVisibleExcess(node);
	edge->Tag = (void*) 1;
}

void KR1992::UpdateVisibleExcess(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	if (tag->excess > tag->hiddenCapacities)
	{
		bool hadNoVisibleExcess = tag->visibleExcess == 0;
		tag->visibleExcess = tag->excess - tag->hiddenCapacities;
		if (hadNoVisibleExcess && tag->visibleExcess > 0 && node != source && node != target)
			activeNodes.AddLast(node);
	}
	else
		tag->visibleExcess = 0;
}

void KR1992::Saturate(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	NodeTag* tag = node->GetTag<NodeTag*>();
	NodeTag* targetTag = edge->TargetNode->GetTag<NodeTag*>();

	if (forest->GetLink(node) == edge->TargetNode)
	{			
		uint32_t cost = forest->GetCost(node);
		tag->excess -= cost;
		UpdateVisibleExcess(node);
		targetTag->excess += cost;
		UpdateVisibleExcess(edge->TargetNode);
		forest->SetCost(node, 0);
	}
	else
	{
		tag->excess -= edge->Capasity;
		targetTag->excess += edge->Capasity;

		//If not added
		if (!edge->Tag)
		{
			tag->hiddenCapacities -= edge->Capasity;
			targetTag->hiddenCapacities += edge->Capasity;
		}
		UpdateVisibleExcess(node);
		UpdateVisibleExcess(edge->TargetNode);

		edge->LinkedEdge->Capasity += edge->Capasity;
		edge->Capasity = 0;

	}

	game->EdgeKill(edge, tag->label);
}

void KR1992::Relable(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	game->NodeKill(node, tag->label);
	tag->label++;

}

void KR1992GlobalRename::Relable(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();
	
	KR1992::Relable(node);
	


	while (true)
	{
		Edge* currentEdge = game->CurrentEdge(node, tag->label);
		if (currentEdge == 0)
		{
			GlobalRelabel();
			break;
		}
		if (currentEdge->Capasity == 0)
			Saturate(currentEdge);
		else 
			break;
	}
}

void KR1992GlobalRename::GlobalRelabel()
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
				if (requiredLabel > neighborTag->label + 1)
					requiredLabel = neighborTag->label + 1;
			}
		}

		while (requiredLabel > tag->label)
			Relable(node);
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
				if (requiredLabel > neighborTag->label + 1)
					requiredLabel = neighborTag->label + 1;
			}
		}

		while (requiredLabel > tag->label)
			Relable(node);
	}
	
	
	performingGlobalRelabel = false;
}


void KR1992RareGlobalRename::Initialize()
{
	passEndNode = 0;
	excessLastPass = 0;
	excessLastRelabel = 0;
	KR1992GlobalRename::Initialize();
}

void KR1992RareGlobalRename::Relable(Node* node)
{
	KR1992::Relable(node);
}

void KR1992RareGlobalRename::GlobalRelabel()
{
	KR1992GlobalRename::GlobalRelabel();
	NodeTag* tag = target->GetTag<NodeTag*>();
	/*if (activeNodes.GetCount() == 0)
		passEndNode = 0;
	else
		passEndNode = activeNodes.GetTail()->GetInstance();*/
	excessLastPass = tag->excess;
	excessLastRelabel = tag->excess;
}

void KR1992RareGlobalRename::FinishProcessingNode(Node* node)
{
	/*if (passEndNode != 0 && node != passEndNode && passEndNode != source && passEndNode != target)
		return;*/
	
	nodesProcessed++;
	if (nodesProcessed < graph->NodeCount/4)
		return;
	nodesProcessed = 0;
	NodeTag* tag = target->GetTag<NodeTag*>();
	if (excessLastPass == tag->excess && excessLastRelabel != tag->excess)
		GlobalRelabel();
	else
	{
		/*if (activeNodes.GetCount() == 0)
			passEndNode = 0;
		else
			passEndNode = activeNodes.GetTail()->GetInstance();*/
		excessLastPass = tag->excess;
	}
}


void KR1992::TreePush(Node* node)
{
	NodeTag* tag = node->GetTag<NodeTag*>();

	Node* bounding = forest->GetBoundingNode(node, tag->visibleExcess);
	if (bounding != NULL)
	{
		NodeTag* tag2 = bounding->GetTag<NodeTag*>();
		Edge* edge = game->CurrentEdge(bounding, tag2->label);
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


KR1992::Game::Game(KR1992* alg)
{
	this->alg = alg;
	this->graph = alg->graph;
	this->forest = alg->forest;

	
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
	resetting = false;

	sourceNodes = new SourceNode[graph->NodeCount*graph->NodeCount*2];
	targetNodes = new TargetNode[graph->NodeCount*graph->NodeCount*2];
	edges = new GameEdge[graph->EdgeCount*graph->NodeCount*2];
	LinkedList<GameEdge*>::Allocate(graph->EdgeCount*graph->NodeCount*2*3);

	for (uint32_t level = 0; level < graph->NodeCount*2; level++)
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		SourceNode* source = GetSource(node, level);
		source->degree = node->EdgeCount;
		if (level == 0)
			source->degree = 0;
		source->edgesByErl = new LinkedList<GameEdge*>[t + 1];
		source->designatedEdge = 0;
		source->edgeLookup = new GameEdge*[node->EdgeCount];
		for (int i = 0; i < node->EdgeCount; i++)
			source->edgeLookup[i] = 0;

		TargetNode* target = GetTarget(node, level);
		target->designatedDegree = 0;
		target->rl = 0;
		target->erl = 0;
	}

	GameEdge* gameEdge = edges;
	for (uint32_t level = 1; level < graph->NodeCount*2; level++)
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++, gameEdge++)
	{
		gameEdge->source = GetSource(edge->LinkedEdge->TargetNode, level);
		gameEdge->target = GetTarget(edge->TargetNode, level - 1);
		gameEdge->edge = edge;
		
		gameEdge->edgeListTag = gameEdge->source->edges.AddFirst(gameEdge);
		gameEdge->edgeByErlTag = gameEdge->source->edgesByErl[0].AddFirst(gameEdge);
		gameEdge->targetTag = gameEdge->target->edges.AddFirst(gameEdge);
		gameEdge->source->edgeLookup[edge - node->Edges] = gameEdge;
	}

	resetting = true;
	for (SourceNode* node = sourceNodes; node < sourceNodes  + graph->NodeCount*graph->NodeCount*2; node++)
	{
		DesignateEdge(node);
	}
	resetting = false;
	for (TargetNode* node = targetNodes; node < targetNodes  + graph->NodeCount*graph->NodeCount*2; node++)
	{
		if (node->erl >= t)
		{
			Reset();
			return;
		}
	}
	
}

KR1992::Game::~Game()
{
	for (SourceNode* node = sourceNodes; node < sourceNodes  + graph->NodeCount*graph->NodeCount*2; node++)
	{
		delete[] node->edgesByErl;
		delete[] node->edgeLookup;
	}
	
	delete[] sourceNodes;
	delete[] targetNodes;
	delete[] edges;
}

Edge* KR1992::Game::CurrentEdge(Node* node, uint32_t label)
{
	SourceNode* source = GetSource(node, label);
	if (source->designatedEdge == NULL)
		return NULL;
	return source->designatedEdge->edge;
}

void KR1992::Game::EdgeKill(Edge* edge, uint32_t label)
{
	Node* node = edge->LinkedEdge->TargetNode;
	SourceNode* source = GetSource(node, label);
	GameEdge* gameEdge = source->edgeLookup[edge - node->Edges];
	TargetNode* target = gameEdge->target;

	source->degree--;
	source->edges.Remove(gameEdge->edgeListTag);
	source->edgesByErl[target->erl].Remove(gameEdge->edgeByErlTag);
	target->edges.Remove(gameEdge->targetTag);

	if (gameEdge == source->designatedEdge)
	{
		if (source->degree >= l - 1)
		{
			target->designatedDegree--;
			UpdateRl(target);
		}
		DesignateEdge(source);
	}
}

void KR1992::Game::NodeKill(Node* node, uint32_t label)
{
	TargetNode* target = GetTarget(node, label);
	for (LinkedList<GameEdge*>::Node* e = target->edges.GetHead(); e != NULL; )
	{
		GameEdge* gameEdge = e->GetInstance();
		e = e->GetNext();

		SourceNode* source = gameEdge->source;
		source->degree--;
		source->edges.Remove(gameEdge->edgeListTag);
		source->edgesByErl[target->erl].Remove(gameEdge->edgeByErlTag);
		target->edges.Remove(gameEdge->targetTag);

		if (gameEdge == source->designatedEdge)
		{
			if (source->degree >= l - 1)
			{
				target->designatedDegree--;
				UpdateRl(target);
			}
			DesignateEdge(source);
		}
	}
	Cut(node, label);
	Link(node, label + 1);
}

void KR1992::Game::DesignateEdge(SourceNode* node)
{
	Cut(node);
	node->designatedEdge = 0;
	if (node->degree < l)
	{
		if (!node->edges.IsEmpty())
		{
			node->designatedEdge = node->edges.GetHead()->GetInstance();
			Link(node);
		}
		return;
	}
	for (int i = 0; i <= t; i++)
	{
		if (node->edgesByErl[i].IsEmpty())
			continue;
		node->designatedEdge = node->edgesByErl[i].GetHead()->GetInstance();
		break;
	}
	if (node->designatedEdge == 0)
		return;
	Link(node);
	node->designatedEdge->target->designatedDegree++;
	UpdateRl(node->designatedEdge->target);
}

void KR1992::Game::UpdateRl(TargetNode* node)
{
	node->rl = 0;
	float ratio = node->designatedDegree/(float)GetNode(node)->EdgeCount;
	float r = r0;
	while (ratio > r)
	{
		r *= 2;
		node->rl++;
	}
	if (node->rl > t)
		throw "Rl bigger than t";
	if (node->rl > node->erl ||
		node->rl + 2 < node->erl)
		UpdateErl(node);
}

void KR1992::Game::UpdateErl(TargetNode* node)
{
	if (node->erl == node->rl)
		return;
	for (LinkedList<GameEdge*>::Node* e = node->edges.GetHead(); e != NULL; e = e->GetNext())
	{
		GameEdge* edge = e->GetInstance();
		SourceNode* source = edge->source;
		source->edgesByErl[node->rl].MoveFirst(source->edgesByErl + node->erl, edge->edgeByErlTag);
	}

	node->erl = node->rl;
	if (node->erl == t)
		Reset();
}

void KR1992::Game::Reset()
{
	if (t <= 2)
		return;
	if (resetting)
		return;
	resetting = true;
	//Build Vk and Uk
	LinkedList<TargetNode*>* targetsByRl = new LinkedList<TargetNode*>[t + 1];
	LinkedList<SourceNode*>* sourcesByRl = new LinkedList<SourceNode*>[t + 1];
	
	for (TargetNode* node = targetNodes; node < targetNodes  + graph->NodeCount*graph->NodeCount*2; node++)
	{
		targetsByRl[node->rl].AddFirst(node);
	}
	for (SourceNode* node = sourceNodes; node < sourceNodes  + graph->NodeCount*graph->NodeCount*2; node++)
	{
		if (node->degree < l)
			continue;
		sourcesByRl[node->designatedEdge->target->rl].AddFirst(node);
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
	for (LinkedList<TargetNode*>::Node* linkedListElement = targetsByRl[k - 2].GetHead(); linkedListElement != NULL; linkedListElement = linkedListElement->GetNext())
	{
		UpdateErl(linkedListElement->GetInstance());
	}
	//Undesignate edges
	for (int i = k; i <= t; i++)
	{
		for (LinkedList<SourceNode*>::Node* linkedListElement = sourcesByRl[i].GetHead(); linkedListElement != NULL; linkedListElement = linkedListElement->GetNext())
		{
			SourceNode* node = linkedListElement->GetInstance();
			TargetNode* target = node->designatedEdge->target;
			node->designatedEdge = NULL;
			target->designatedDegree--;
		}
	}
	//Update target ERL
	for (int i = k; i <= t; i++)
	{
		for (LinkedList<TargetNode*>::Node* linkedListElement = targetsByRl[i].GetHead(); linkedListElement != NULL; linkedListElement = linkedListElement->GetNext())
		{
			UpdateRl(linkedListElement->GetInstance());
		}
	}
	//Designate edges
	for (int i = k; i <= t; i++)
	{
		for (LinkedList<SourceNode*>::Node* linkedListElement = sourcesByRl[i].GetHead(); linkedListElement != NULL; linkedListElement = linkedListElement->GetNext())
		{
			DesignateEdge(linkedListElement->GetInstance());
		}
	}

	delete[] targetsByRl;
	delete[] sourcesByRl;
	delete[] vCounts;
	delete[] uCounts;
	resetting = false;
}

void KR1992::Game::Cut(SourceNode* source)
{
	Node* node = GetNode(source);
	uint32_t level = node->GetTag<NodeTag*>()->label;
	if (level == GetLevel(source))
		Cut(node, level);
}

void KR1992::Game::Link(SourceNode* source)
{
	Node* node = GetNode(source);
	uint32_t level = node->GetTag<NodeTag*>()->label;
	if (level == GetLevel(source))
		Link(node, level);
}

void KR1992::Game::Cut(Node* node, uint32_t level)
{
	if (forest->IsLinked(node))
	{
		Edge* currentEdge = CurrentEdge(node, level);
		uint32_t cap = forest->GetCost(node);
		uint32_t diff = currentEdge->Capasity - cap;
		
		//If not added
		if (!currentEdge->Tag)
		{
			node->GetTag<NodeTag*>()->hiddenCapacities -= diff;
			currentEdge->TargetNode->GetTag<NodeTag*>()->hiddenCapacities += diff;
			alg->UpdateVisibleExcess(node);
			alg->UpdateVisibleExcess(currentEdge->TargetNode);
		}

		currentEdge->LinkedEdge->Capasity += diff;
		currentEdge->Capasity = cap;
		forest->Cut(node);
	}
}

void KR1992::Game::Link(Node* node, uint32_t level)
{
	Edge* currentEdge = CurrentEdge(node, level);
	if (forest->IsLinked(node))
		throw "ARARARS";
	if (currentEdge != NULL)
	{
		forest->SetCost(node, currentEdge->Capasity);
		forest->Link(node, currentEdge->TargetNode);
	}
}

uint32_t KR1992::GetCapacity(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	uint32_t capacity = edge->Capasity;
	if (forest->IsLinked(node) && edge->TargetNode == forest->GetLink(node))
		capacity = forest->DebugGetCost(node);
	if (forest->IsLinked(edge->TargetNode) && node == forest->GetLink(edge->TargetNode))
		capacity += edge->LinkedEdge->Capasity - forest->DebugGetCost(edge->TargetNode);
	return capacity;
}


