

#include "KingRaoTarjan1994.h"
#include "../../Utilities/Debug.h"



uint32_t KingRaoTarjan1994::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	assert(x >= 2);
	this->graph = graph;
	this->source = source;
	this->target = target;
	flow = 0;
	Initialize();
	for (Edge** ppEdge = edgesNotYetAdded; ppEdge < edgesNotYetAdded + edgesNotYetAddedCount; ppEdge++)
	{
		Edge* edge = *ppEdge;
		if (IsActive(edge))
			continue;
		int from = edge->LinkedEdge->TargetNode - graph->Nodes + 1;
		int to = edge->TargetNode - graph->Nodes + 1;
		SetActive(edge);

		//Add edge and linkedEdge to dynamic forest
		Node* source = edge->LinkedEdge->TargetNode;
		Node* target = edge->TargetNode;
		
		KRTNodeTag* sourceTag = source->GetTag<KRTNodeTag*>();
		KRTNodeTag* targetTag = target->GetTag<KRTNodeTag*>();


		if (targetTag->level < sourceTag->level)
			EdgePush(edge, edge->Capasity);
		else if (sourceTag->level < targetTag->level)
			EdgePush(edge->LinkedEdge, edge->LinkedEdge->Capasity);
		
		while (!nodeQueue.IsEmpty())
		{
			Node* node = nodeQueue.GetTail()->GetInstance();
			if (node == this->source || node == this->target || node->GetTag<KRTNodeTag*>()->visibleExcess == 0)
			{
				nodeQueue.RemoveLast();
				continue;
			}

			Update(GetSourceNode(node));
			KRTNodeTag* tag = node->GetTag<KRTNodeTag*>();
			while (tag->visibleExcess > 0)
			{
				Edge* currentEdge = CurrentEdge(node);
				if (currentEdge == NULL)
					Relable(node);
				else
					Push(node);
			}
		}

	}
	CleanUp();
	return flow;
}

void KingRaoTarjan1994::Initialize()
{
	//O(nm)
	InitializeNodeTags();
	//O(nm)
	InitializeGameBoard();

	InitializeEdgeList();
}


int edgeComparator(const void * a, const void * b)
{
	Edge* edge1 = (Edge*)a;
	Edge* edge2 = (Edge*)b;

	int64_t edge1Capacity = edge1->Capasity + edge1->LinkedEdge->Capasity;
	int64_t edge2Capacity = edge2->Capasity + edge2->LinkedEdge->Capasity;

	int cmp = edge1Capacity - edge2Capacity;
	if (cmp != 0)
		return cmp;
	
	if (edge1 > edge1->LinkedEdge)
		edge1 = edge1->LinkedEdge;
	if (edge2 > edge2->LinkedEdge)
		edge2 = edge2->LinkedEdge;
	return edge1 - edge2;
}


int edgepComparator(const void * a, const void * b)
{
	Edge** edge1 = (Edge**)a;
	Edge** edge2 = (Edge**)b;
	return edgeComparator(*edge1, *edge2);
}

//O(nm)
void KingRaoTarjan1994::InitializeNodeTags()
{
	Node* node = graph->Nodes;
	for (int i = 0; i < graph->NodeCount; i++, node++)
	{
		KRTNodeTag* tag = new KRTNodeTag();
		tag->level = 0;
		
		tag->excess = 0;
		tag->visibleExcess = 0;
		tag->hiddenCapacities = 0;
		tag->linkedListNode = 0;

		for (int j = 0; j < node->EdgeCount; j++)
		{
			tag->hiddenCapacities += node->Edges[j].Capasity;
			node->Edges[j].Tag = 0;
		}

		node->Tag = tag;
	}
}

void KingRaoTarjan1994::InitializeEdgeList()
{
	AddSourceEdges();
	edgesNotYetAddedCount = graph->EdgeCount/2 - source->EdgeCount;
	edgesNotYetAdded = new Edge*[edgesNotYetAddedCount];
	Edge** p = edgesNotYetAdded;
	EdgeEnumerator enumerator(graph);
	while (enumerator.HasNext())
	{
		enumerator.Next();
		Edge* edge = enumerator.GetCurrent();
		if (edge->TargetNode == source || edge->LinkedEdge->TargetNode == source)
			continue;
		int sourceNodeIndex = edge->LinkedEdge->TargetNode - graph->Nodes;
		int targetNodeIndex = edge->TargetNode - graph->Nodes;
		if (sourceNodeIndex < targetNodeIndex)
			*p++ = edge;
	}
	qsort(edgesNotYetAdded, edgesNotYetAddedCount, sizeof(Edge*), edgepComparator);
	for (int i = 0; i < edgesNotYetAddedCount/2; i++)
	{
		Edge* temp = edgesNotYetAdded[i];
		edgesNotYetAdded[i] = edgesNotYetAdded[edgesNotYetAddedCount - i - 1];
		edgesNotYetAdded[edgesNotYetAddedCount - i - 1] = temp;
	}
}

void KingRaoTarjan1994::AddSourceEdges()
{
	for (Edge* edge = source->Edges; edge < source->Edges + source->EdgeCount; edge++)
	{		
		//O(n)
		SetActive(edge);
		//O(n^2)
		GameForceUpdate(edge->TargetNode);
		//O(n^2)
		GameForceUpdate(source);
	}
	
	KRTNodeTag* tag = source->GetTag<KRTNodeTag*>();
	Update(GetSourceNode(source));
	Relable(source);
	Edge* currentEdge = CurrentEdge(source);
	while (currentEdge != 0)
	{
		EdgePush(currentEdge, currentEdge->Capasity);
		currentEdge = CurrentEdge(source);
	}

	while (tag->level < graph->NodeCount)
	{
		Relable(source);
	}
}

//O(nm)
void KingRaoTarjan1994::InitializeGameBoard()
{
	uint32_t levelCount = 2*graph->NodeCount-1;
	t = 3*(uint32_t)ceil(log(graph->NodeCount*levelCount)/(log(r0*l) - log(88) - log(x))) + 4;
	gameSourceNodes = new GameSourceNode*[graph->NodeCount];
	gameSourceNodes[0] = new GameSourceNode[levelCount*graph->NodeCount];
	//O(n)
	for (int i = 0; i < graph->NodeCount; i++)
		gameSourceNodes[i] = gameSourceNodes[0] + i*levelCount;
	
	gameTargetNodes = new GameTargetNode*[graph->NodeCount];
	gameTargetNodes[0] = new GameTargetNode[levelCount*graph->NodeCount];
	//O(n)
	for (int i = 0; i < graph->NodeCount; i++)
		gameTargetNodes[i] = gameTargetNodes[0] + i*levelCount;

	//O(n^2)
	allGameEdges = new GameEdge[levelCount*graph->EdgeCount];
	GameEdge* nextGameEdge = allGameEdges;
	for (int n = 0; n < graph->NodeCount; n++)
	for (int k = 0; k < levelCount; k++)
	{
		GameTargetNode* target = gameTargetNodes[n] + k;
		target->DesignatedEdgeCount = 0;
		target->EstimatedRatioLevel = 0;
		target->RatioLevel = 0;
		Node* node = graph->Nodes + n;
		//O(nm) total
		for (int e = 0; e < node->EdgeCount; e++)
		{
			GameEdge* gameEdge = nextGameEdge++;
			gameEdge->target = target;
			gameEdge->targetElement = target->Neighbors.AddFirst(gameEdge);
		}
	}

	Edge** edgeArray = new Edge*[1];
	uint32_t edgeArrayCount = 1;
	//O(nm)
	for (int n = 0; n < graph->NodeCount; n++)
	{
		Node* node = graph->Nodes + n;
		if (node->EdgeCount > edgeArrayCount)
		{
			//O(log(m)) total
			do
			{
				edgeArrayCount *= 2;
			} while (node->EdgeCount > edgeArrayCount);
			delete[] edgeArray;
			edgeArray = new Edge*[edgeArrayCount];
		}
		
		//O(m) total
		for (int e = 0; e < node->EdgeCount; e++)
			edgeArray[e] = node->Edges + e;
		
		//O(m*log(m))=O(m*log(n)) total
		qsort(edgeArray, node->EdgeCount, sizeof(Edge*), edgepComparator);

		for (int k = 0; k < levelCount; k++)
		{
			GameSourceNode* source = gameSourceNodes[n] + k;
			source->Neighbors = new LinkedList<GameEdge*>[t + 1];
			source->DesignatedEdge = 0;
			source->isInU = false;
			source->removedEdges = 0;
			source->workingEdges = 0;
			source->edgeMap = new GameEdge*[node->EdgeCount];
			
			if (k == 0)
				continue;
				
			//O(m) total
			for (int e = 0; e < node->EdgeCount; e++)
			{
				Edge* edge = edgeArray[e];
				GameTargetNode* targetNode = GetTargetNode(edge->TargetNode, k - 1);
				GameEdge* gameEdge = targetNode->Neighbors.RemoveFirst();
				gameEdge->edge = edge;
				gameEdge->source = source;
				gameEdge->sourceElement = source->InvalidNeighbors.AddLast(gameEdge);
				gameEdge->targetElement = targetNode->Neighbors.AddLast(gameEdge);
				source->edgeMap[edge - node->Edges] = gameEdge;
			}
		}
	}
	delete[] edgeArray;
}

void KingRaoTarjan1994::CleanUp()
{
	IsCleaning = true;
	for (int n = 0; n < graph->NodeCount; n++)
	{
		for (int k = 0; k < 2*graph->NodeCount-1; k++)
		{
			{
				GameTargetNode* node = GetTargetNode(graph->Nodes + n, k);
				node->Neighbors.Clear();
			}
			{
				GameSourceNode* source = GetSourceNode(graph->Nodes + n, k);
				source->InvalidNeighbors.Clear();
				for (int i = 0; i < t + 1; i++)
					source->Neighbors[i].Clear();
				delete[] source->Neighbors;
				delete[] source->edgeMap;
			}
		}
		{
			Node* node = graph->Nodes + n;
			delete node->GetTag<KRTNodeTag*>();
			node->Tag = 0;
		}
	}
	delete[] allGameEdges;
	delete[] gameSourceNodes[0];
	delete[] gameSourceNodes;
	delete[] gameTargetNodes[0];
	delete[] gameTargetNodes;
	delete[] edgesNotYetAdded;
}


void KingRaoTarjan1994::EdgePush(Edge* edge, uint32_t capacity)
{
	edge->Capasity -= capacity;
	edge->LinkedEdge->Capasity += capacity;
	KRTNodeTag* tag = edge->TargetNode->GetTag<KRTNodeTag*>();
	tag->excess += capacity;
	tag->UpdateVisibleExcess();
	if (edge->TargetNode == target)
	{
		tag->visibleExcess = 0;
		flow += capacity;
	}
	if (tag->linkedListNode == 0 && tag->visibleExcess > 0)
		tag->linkedListNode = nodeQueue.AddFirst(edge->TargetNode);

	tag = edge->LinkedEdge->TargetNode->GetTag<KRTNodeTag*>();
	tag->excess -= capacity;
	tag->UpdateVisibleExcess();
	if (tag->linkedListNode != 0 && tag->visibleExcess == 0)
	{
		nodeQueue.Remove(tag->linkedListNode);
		tag->linkedListNode = 0;
	}
	if (edge->Capasity == 0)
	{
		GameSaturatedPush(edge);
	}
}

void KingRaoTarjan1994::Relable(Node* node)
{
	GameRelabelingNode(node);
	KRTNodeTag* tag = node->GetTag<KRTNodeTag*>();
	tag->level++;
	Update(GetSourceNode(node));
}

void KingRaoTarjan1994::Push(Node* node)
{
	KRTNodeTag* tag = node->GetTag<KRTNodeTag*>();
	Edge* edge = CurrentEdge(node);

	uint32_t flow = tag->visibleExcess;

	while (edge != NULL)
	{
		if (edge->Capasity < flow)
		{
			EdgePush(edge, edge->Capasity);
			break;
		}
		EdgePush(edge, flow);
		edge = CurrentEdge(edge->TargetNode);
	}
}


Edge* KingRaoTarjan1994::CurrentEdge(Node* node)
{
	GameSourceNode* gameNode = GetSourceNode(node);
	GameEdge* gameEdge = gameNode->DesignatedEdge;
	if (gameEdge == NULL)
		return NULL;
	return gameEdge->edge;
}


KingRaoTarjan1994::GameSourceNode* KingRaoTarjan1994::GetSourceNode(Node* node)
{
	return GetSourceNode(node, node->GetTag<KRTNodeTag*>()->level);
}

KingRaoTarjan1994::GameSourceNode* KingRaoTarjan1994::GetSourceNode(Node* node, uint32_t level)
{
	return gameSourceNodes[node - graph->Nodes] + level;
}

KingRaoTarjan1994::GameTargetNode* KingRaoTarjan1994::GetTargetNode(Node* node)
{
	return GetTargetNode(node, node->GetTag<KRTNodeTag*>()->level);
}

KingRaoTarjan1994::GameTargetNode* KingRaoTarjan1994::GetTargetNode(Node* node, uint32_t level)
{
	return gameTargetNodes[node - graph->Nodes] + level;
}

Node* KingRaoTarjan1994::GetGraphNode(GameTargetNode* node)
{
	return graph->Nodes + (node - gameTargetNodes[0])/(graph->NodeCount*2-1);
}

Node* KingRaoTarjan1994::GetGraphNode(GameSourceNode* node)
{
	return graph->Nodes + (node - gameSourceNodes[0])/(graph->NodeCount*2-1);
}


int KingRaoTarjan1994::GetK(GameTargetNode* node)
{
	return (node - gameTargetNodes[0])%(graph->NodeCount*2-1);
}

int KingRaoTarjan1994::GetK(GameSourceNode* node)
{
	return (node - gameSourceNodes[0])%(graph->NodeCount*2-1);
}

bool KingRaoTarjan1994::IsActive(Edge* edge)
{
	return edge->Tag;
}

void KingRaoTarjan1994::SetActive(Edge* edge)
{
	while (!IsActive(edge))
	{
		if (edge->TargetNode - graph->Nodes == 1)
			edge->Tag = (void*)2;
		edge->Tag = (void*)1;
		KRTNodeTag* tag = edge->LinkedEdge->TargetNode->GetTag<KRTNodeTag*>();
		tag->hiddenCapacities -= edge->Capasity;
		bool hadVisibleExcess = tag->visibleExcess > 0;
		tag->UpdateVisibleExcess();
		tag->activeEdges++;
		if (!hadVisibleExcess && tag->visibleExcess > 0)
			nodeQueue.AddFirst(edge->LinkedEdge->TargetNode);

		

		edge = edge->LinkedEdge;
	}
}

uint32_t KingRaoTarjan1994::GetId(Node* node)
{
	return node - graph->Nodes;
}

void KingRaoTarjan1994::GameSaturatedPush(Edge* edge)
{
	Node* node = edge->LinkedEdge->TargetNode;
	GameSourceNode* source = GetSourceNode(node);
	GameEdge* gameEdge = source->edgeMap[edge - node->Edges];
	GameRemoveEdge(gameEdge);
}

void KingRaoTarjan1994::GameRelabelingNode(Node* node)
{
	GameTargetNode* oldTarget = GetTargetNode(node);
	GameRemoveNode(oldTarget);
}

void KingRaoTarjan1994::GameRemoveNode(GameTargetNode* node)
{
	LinkedList<GameEdge*>::Node* curr = node->Neighbors.GetHead();
	LinkedList<GameEdge*>::Node* next = curr;
	while (curr != 0)
	{
		next = next->GetNext();
		GameRemoveEdge(curr->GetInstance());
		curr = next;
	}
}


void KingRaoTarjan1994::GameUpdateRatioLevel(GameTargetNode* node)
{
	node->RatioLevel = 0;
	if (GetGraphNode(node)->EdgeCount < l)
		return;
	float ratio = node->DesignatedEdgeCount / GetGraphNode(node)->EdgeCount;
	float r = r0;
	for (; node->RatioLevel < t; node->RatioLevel++)
	{
		if (ratio < r)
			break;
		r *= 1 + 1/x;
	}
}

void KingRaoTarjan1994::GameUpdateEstimatedRatioLevel(GameTargetNode* node)
{
	uint32_t oldErl = node->EstimatedRatioLevel;
	uint32_t newErl = node->RatioLevel;
	node->EstimatedRatioLevel = newErl;
	if (oldErl == newErl)
		return;
	for (LinkedList<GameEdge*>::Node* element = node->Neighbors.GetHead(); element != NULL; element = element->GetNext())
	{
		GameEdge* edge = element->GetInstance();
		GameSourceNode* source = edge->source;
		Update(source);
		if (!source->isInU)
			continue;
		if (IsActive(edge->edge))
			source->Neighbors[newErl].MoveFirst(source->Neighbors + oldErl, edge->sourceElement);
		else
			source->Neighbors[newErl].MoveLast(source->Neighbors + oldErl, edge->sourceElement);
	}
}

void KingRaoTarjan1994::Update(GameSourceNode* node)
{
	//Every edge is only added once pr layer. O(nm) total
	bool addedEdges = false;
	while (!node->InvalidNeighbors.IsEmpty())
	{
		int nodeIndex = GetGraphNode(node) - graph->Nodes;
		GameEdge* edge = node->InvalidNeighbors.GetTail()->GetInstance();
		if (IsActive(edge->edge))
		{
			int index = node->isInU ? edge->target->EstimatedRatioLevel : 0;
			node->Neighbors[index].MoveFirst(&node->InvalidNeighbors, node->InvalidNeighbors.GetTail());
			addedEdges = true;
		}
		else
			break;
	}
	
	//O(n) every time a nodes shouldBeInU changes
	//An edge is only added or removed once, resulting in O(n) changes to the number of active edges for a node.
	//There are O(n^2) nodes in the game => O(n^4) time total :(
	int activeEdges = GetGraphNode(node)->EdgeCount - node->removedEdges - node->InvalidNeighbors.GetCount();
	bool shouldBeInU = activeEdges >= l;
	if (shouldBeInU != node->isInU)
	{
		if (shouldBeInU)
		{
			GameEdge** edges = new GameEdge*[activeEdges];	
			GameEdge** e = edges;
			for (LinkedList<GameEdge*>::Node* elm = node->Neighbors[0].GetHead(); elm != NULL; elm = elm->GetNext(), e++)
				*e = elm->GetInstance();
			for (e = edges; e < edges + activeEdges; e++)
			{
				GameEdge* edge = *e;
				node->Neighbors[edge->target->EstimatedRatioLevel].MoveFirst(node->Neighbors, edge->sourceElement);
			}
			delete[] edges;
		}
		else
		{
			for (int k = 1; k <= t; k++)
			{
				while (!node->Neighbors[k].IsEmpty())
					node->Neighbors[0].MoveFirst(node->Neighbors + k, node->Neighbors[k].GetHead());
			}
		}
		node->isInU = shouldBeInU;
	}
	
	//This can only happen O(n) times pr node => O(n^3)
	//If it happens, there can only be one active edge for the node => constant time for designation.
	if (addedEdges && node->DesignatedEdge == NULL)
		GameDesignateEdge(node);
}

void KingRaoTarjan1994::GameForceUpdate(Node* graphNode)
{
	for (int k = 0; k < 2*graph->NodeCount-1; k++)
	{
		GameSourceNode* node = GetSourceNode(graphNode, k);
		bool addedEdges = false;
		LinkedList<GameEdge*>::Node* edge = node->InvalidNeighbors.GetHead();
		LinkedList<GameEdge*>::Node* next = edge;
		while (edge != NULL)
		{
			next = edge->GetNext();
			if (IsActive(edge->GetInstance()->edge))
			{
				int index = node->isInU ? edge->GetInstance()->target->EstimatedRatioLevel : 0;
				node->Neighbors[index].MoveFirst(&node->InvalidNeighbors, edge);
				addedEdges = true;
			}
			edge = next;
		}
		
		if (addedEdges && node->DesignatedEdge == NULL)
			GameDesignateEdge(node);
	}
}


void KingRaoTarjan1994::GameRemoveEdge(GameEdge* edge)
{
	//Remove edge from u's neighbor list
	Update(edge->source);
	int index = edge->source->isInU ? edge->target->EstimatedRatioLevel : 0;
	if (IsActive(edge->edge))
		edge->source->Neighbors[index].Remove(edge->sourceElement);
	else
		edge->source->InvalidNeighbors.Remove(edge->sourceElement);
	edge->sourceElement = 0;
	edge->target->Neighbors.Remove(edge->targetElement);
	edge->targetElement = 0;
	
	edge->source->removedEdges++;
	Update(edge->source);

	if (edge->source->DesignatedEdge == edge)
	{
		if (edge->source->isInU)
		{
			edge->target->DesignatedEdgeCount--;
			GameUpdateRatioLevel(edge->target);
			if (edge->target->RatioLevel > edge->target->EstimatedRatioLevel)
				GameUpdateEstimatedRatioLevel(edge->target);
		}
		edge->source->DesignatedEdge = 0;
		GameDesignateEdge(edge->source);
	}
}

void KingRaoTarjan1994::GameDesignateEdge(GameSourceNode* node)
{
	Update(node);
	node->DesignatedEdge = NULL;
	if (!node->isInU)
	{
		if (!node->Neighbors[0].IsEmpty())
			node->DesignatedEdge = node->Neighbors[0].GetHead()->GetInstance();
		return;
	}
	for (int k = 0; k <= t; k++)
	{
		if (!node->Neighbors[0].IsEmpty())
		{
			node->DesignatedEdge = node->Neighbors[k].GetHead()->GetInstance();
		}
	}
	GameTargetNode* target = node->DesignatedEdge->target;
	target->DesignatedEdgeCount++;
	GameUpdateRatioLevel(target);
	if (target->RatioLevel > target->EstimatedRatioLevel)
		GameUpdateEstimatedRatioLevel(target);
}


/*
void KingRaoTarjan1994::AssertGameConsistency()
{
	for (int n = 0; n < graph->NodeCount; n++)
	for (int k = 0; k < 2*graph->NodeCount-2; k++)
	{
		{
			GameSourceNode* source = GetSourceNode(graph->Nodes + n, k);
			uint32_t value = 0;
			LinkedList<GameEdge*>::Node* curr = source->InvalidNeighbors.GetHead();
			LinkedList<GameEdge*>::Node* next = curr;
			while (next != NULL)
			{
				curr = next;
				next = next->GetNext();
				Edge* edge = curr->GetInstance()->edge;
				uint32_t newValue = edge->Capasity + edge->LinkedEdge->Capasity;
				if (newValue < value)
					throw "Inconsistent!";
				value = newValue;

			}
		}
	}
	for (int n = 0; n < graph->NodeCount; n++)
	for (int k = 0; k < 2*graph->NodeCount-2; k++)
	{
		{
			GameTargetNode* node = GetTargetNode(graph->Nodes + n, k);
			LinkedList<GameEdge*>::Node* curr = node->Neighbors.GetHead();
			LinkedList<GameEdge*>::Node* next = curr;
			while (curr != NULL)
			{
				next = curr->GetNext();

				GameEdge* edge = curr->GetInstance();
				if (edge->edge == NULL)
					throw "Incosistent!";
				if (edge->source == NULL)
					throw "Incosistent!";
				if (edge->sourceElement == NULL)
					throw "Incosistent!";
				if (edge->target != node)
					throw "Incosistent!";
				if (curr != edge->targetElement)
					throw "Incosistent!";
				GameSourceNode* source = edge->source;
				Update(source);
				if (IsActive(edge->edge))
				{
					if (source->InvalidNeighbors.Contains(edge->sourceElement))
					{
						LinkedList<GameEdge*>::Node* curr = source->InvalidNeighbors.GetHead();
						LinkedList<GameEdge*>::Node* next = curr;
						while (next != NULL)
						{
							curr = next;
							next = next->GetNext();
							Edge* edge = curr->GetInstance()->edge;
							uint32_t value = edge->Capasity + edge->LinkedEdge->Capasity;
							Debug << value << ", ";

						}
						GameForceUpdate(GetGraphNode(source));
						throw "Incosistent!";
					}
				}
				else
				{			
					if (source->Neighbors[0].Contains(edge->sourceElement))		
						throw "Incosistent!";
				}
				source->InvalidNeighbors.CheckCount();
				source->Neighbors[0].CheckCount();
				node->Neighbors.CheckCount();

				curr = next;
			}
		}
	}
}


void KingRaoTarjan1994::DebugCheck()
{
	int foo;
	for (Node* node = graph->Nodes; node < graph->Nodes + graph->NodeCount; node++)
	{
		if (node == source || node == target)
			continue;
		GameSourceNode* sourceNode = GetSourceNode(node);
		if (sourceNode->Neighbors[0].GetCount() > 1)
			foo = 2;
	}
}
*/