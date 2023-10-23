#include "BlockingFlow.h"
#include "../../../Utilities/UtilityFunctions.h"
#include <algorithm>
#include "../../../Utilities/Debug.h"
#include "../GoldbergRao1998.h"

using namespace std;

#define PRINT_IMAGES_BF 0
#define HIDE_NO_FLOW_EDGES 0

bool BlockingFlow::CheckFlow(GRSuperNode* n)
{
	if(n == m_sourceSuperNode || n == m_targetSuperNode)
		return true;

	Edge* initialEdge = n->m_currentEdge;
	uint32_t initialEdgeAt = n->currentEdgeAt;

	n->ResetToFirstEdge();
	uint32_t ingoingFlow = 0;
	uint32_t outgoingFlow = 0;

	while(n->m_currentEdge != NULL)
	{
		Edge* e = n->m_currentEdge;
		GRSuperNode* targetSuperNode = e->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode;

		if(targetSuperNode == n)
		{
			n->GetNextEdge();
			continue;
		}

		if(m_forest->GetLink(n) != NULL && m_forest->GetLink(n) == targetSuperNode)
		{
			uint32_t treeCost = m_forest->DebugGetCost(n);
			if(n->treeEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == n) //Pull edge
			{
				ingoingFlow += treeCost;
			}
			else
			{
				outgoingFlow += n->treeEdge->Capasity - treeCost;
			}
		}
		else if(m_forest->GetLink(targetSuperNode) == n && (targetSuperNode->treeEdge == e || targetSuperNode->treeEdge == e->LinkedEdge))
		{
			uint32_t treeCost = m_forest->DebugGetCost(targetSuperNode);
			if(targetSuperNode->treeEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == targetSuperNode) //Pull edge
			{
				outgoingFlow += treeCost;
			}
			else
			{
				ingoingFlow += targetSuperNode->treeEdge->Capasity - treeCost;
			}
		} else
		{
			outgoingFlow += e->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
			ingoingFlow += e->LinkedEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
		}
		n->GetNextEdge();
	}
	n->m_currentEdge = initialEdge;
	n->currentEdgeAt = initialEdgeAt;
	if(ingoingFlow != outgoingFlow + n->excess)
		return false;
	return true;
}

BlockingFlow::BlockingFlow(uint32_t nrNodes)
{
	m_queue = new Queue<GRSuperNode*>(nrNodes);
	m_blocked = new bool[nrNodes];
	m_maxNodes = nrNodes;
	if(PRINT_IMAGES_BF)
		nodesDrawed = new bool[nrNodes];
}

BlockingFlow::~BlockingFlow()
{
	delete m_queue;
	delete[] m_blocked;
	if(PRINT_IMAGES_BF)
		delete[] nodesDrawed;
}

void BlockingFlow::ResetTags(uint32_t resetCount)
{
	for(int i = 0; i < resetCount; i++)
	{
		m_blocked[i] = false;
	}
}

inline bool BlockingFlow::EdgeLength(Edge* e)
{
	int distanceDifference = e->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->distance - e->LinkedEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->distance;
	return !((e->Capasity >= m_delta * 2) || (e->Capasity >= m_delta && e->LinkedEdge->Capasity >= 2*m_delta && distanceDifference == 0));
}

void BlockingFlow:: TransferChangesFromDynamicTree(GRSuperNode* superNodes,uint32_t nrSCCs)
{
	for(GRSuperNode* superNode = superNodes; superNode < superNodes + nrSCCs; superNode++)
	{
		if(m_forest->GetLink(superNode) != NULL)
		{
			uint32_t treeCost = m_forest->GetCost(superNode);
			if(superNode->treeEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == superNode) //Pull edge
			{
				uint32_t* edgeFlow = &superNode->treeEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
				*edgeFlow = treeCost;
				
			}
			else
			{
				uint32_t* edgeFlow = &superNode->treeEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
				*edgeFlow = superNode->treeEdge->Capasity - treeCost;
			}
		}
	}
}

uint32_t BlockingFlow::CalculateBlockingFlow(Graph* graph, GRSuperNode* superNodes, Node* source, Node*  target, uint32_t delta, uint32_t nrStronglyConnectecomponents, uint32_t runNr,  bool checkOutput)
{
	m_runNrBF = runNr;
	m_delta = delta;
	m_graph = graph;
	m_forest = new DynamicForest<GRSuperNode>(superNodes, nrStronglyConnectecomponents);
	m_graphNrBF = 0;

	ResetTags(nrStronglyConnectecomponents);
	if(m_queue->GetSize() != 0)
		throw new std::exception("Queue is not empty");

	m_maxSize = graph->NodeCount * graph->NodeCount / graph->EdgeCount;

	GRSuperNode* sourceSuperNode = source->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	while(sourceSuperNode->m_currentEdge != NULL)
	{
		Edge* e = sourceSuperNode->m_currentEdge;
		if(e->Capasity!= 0)
		{
			GRSuperNode* endPointSuperNode = e->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode;
			if(endPointSuperNode->id != sourceSuperNode->id && sourceSuperNode->distance == endPointSuperNode->distance + EdgeLength(e) && endPointSuperNode->distance != UINT32_MAX)
			{
				uint32_t edgeCapacity = e->Capasity;
				e->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = edgeCapacity;
				endPointSuperNode->excess += edgeCapacity;
				if(!endPointSuperNode->addedToQueue)
				{
					m_queue->Enqueue(endPointSuperNode);
					endPointSuperNode->addedToQueue = true;
				}
			}
		}
		sourceSuperNode->GetNextEdge();
	}
	sourceSuperNode->ResetToFirstEdge();

	Discharge(source, target, superNodes, nrStronglyConnectecomponents);

	TransferChangesFromDynamicTree(superNodes, nrStronglyConnectecomponents);

	PrintGraph(m_sourceSuperNode->m_currentEdge);

	if(checkOutput && !TestIsBlockingFlow(source, target))
		return 0;

	delete m_forest;
	return target->GetTag<GoldbergRao1998::GRTag*>()->superNode->excess;
}

void BlockingFlow::Discharge(Node* source, Node* target, GRSuperNode* superNodes, uint32_t nrSccs)
{
	m_sourceSuperNode = source->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	m_targetSuperNode = target->GetTag<GoldbergRao1998::GRTag*>()->superNode;

	Debug << "Start Finding the blocking flow\n";
	while(m_queue->GetSize() != 0)
	{
		GRSuperNode* n = m_queue->Dequeue();

		Debug << "Discharging Super node: " << n->id << "\n";

		//if(n->id != m_tree->FindRoot(n->id)) //Not a root //Non-Roots are allowed since we need to pull and activate the tree in a reverse order.
		//	continue;

		if(n->id == m_sourceSuperNode->id|| n->id == m_targetSuperNode->id)
			continue;
		uint32_t nodeExcess = n->excess;
		bool wasBlocked = m_blocked[n->id];
		while (nodeExcess != 0)
		{
			TreePushPullBlock(n);
			nodeExcess = n->excess;
		}
		if(nodeExcess != 0)	//n still active
			m_queue->Enqueue(n);
	}
	m_sourceSuperNode->ResetToFirstEdge();
	PrintGraph(m_sourceSuperNode->m_currentEdge);
	Debug << "Done Finding the blocking flow\n";
}

void BlockingFlow::TreePushPullBlock(GRSuperNode* v)
{
	Edge* curEdge = v->m_currentEdge;
	Node* to = curEdge->TargetNode;
	uint32_t edgeFlow = curEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
	uint32_t residualCapacity = curEdge->Capasity - edgeFlow;
	GoldbergRao1998::GRTag* toTag = to->GetTag<GoldbergRao1998::GRTag*>();
	GRSuperNode* toSuperNode = toTag->superNode;

	PrintGraph(curEdge);

	if(v->id == toSuperNode->id)
	{
		if(!v->IsLastEdge())
		{
			v->GetNextEdge();
			return;
		}
		else
		{
			Block(v);
			return;
		}
	}
	else if(!m_blocked[v->id] && residualCapacity > 0 && !m_blocked[toSuperNode->id] && v->distance == toSuperNode->distance + EdgeLength(curEdge) && toSuperNode != m_sourceSuperNode && toSuperNode->distance != UINT32_MAX) //Current Edge is push aplicable && admissible
	{
		Push(curEdge);
		SendPush(toSuperNode);
		edgeFlow = curEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
		residualCapacity = curEdge->Capasity - edgeFlow;
		if(residualCapacity > 0 && m_forest->GetPathLength(v) + m_forest->GetPathLength(toSuperNode) <= m_maxSize)
		{
			if(m_forest->GetLink(v) == NULL && m_forest->GetRoot(toSuperNode) != v)
			{
				m_forest->Link(v, toSuperNode);
				m_forest->SetCost(v, residualCapacity);
				v->treeEdge = curEdge;
			}
		}
	}
	else if(m_blocked[v->id] && curEdge->LinkedEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow != 0) //Current Edge is pull aplicable
	//else if (fromTag->blocked && curEdge->LinkedEdge->GetTag<BFEdgeTag*>()->flow != 0) 
	{
		Pull(curEdge);
		SendPull(toSuperNode);
		edgeFlow = curEdge->LinkedEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
		if(edgeFlow > 0 && m_forest->GetPathLength(v) + m_forest->GetPathLength(toSuperNode) <= m_maxSize)
		{
			if(m_forest->GetLink(v) == NULL && m_forest->GetRoot(toSuperNode) != v)
			{
				m_forest->Link(v, toSuperNode);
				m_forest->SetCost(v, edgeFlow);
				v->treeEdge = curEdge->LinkedEdge;
			}
		}
	}
	else
	{
		if(!v->IsLastEdge())
		{
			v->GetNextEdge();
		}
		else
		{
			Block(v);
		}
	}
}

void BlockingFlow::Pull(Edge* e)
{

	Node* from = e->LinkedEdge->TargetNode;
	Node* to = e->TargetNode;
	GRSuperNode* fromSuperNode = from->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	GRSuperNode* toSuperNode = to->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	GoldbergRao1998::GREdgeTag* linkedTag = e->LinkedEdge->GetTag<GoldbergRao1998::GREdgeTag*>();

	uint32_t delta = min(linkedTag->newFlow, fromSuperNode->excess);
	linkedTag->newFlow -= delta;
	fromSuperNode->excess -= delta;

	if(toSuperNode->excess == 0)
	{
		m_queue->Enqueue(toSuperNode);
	}
	toSuperNode->excess += delta;
}

void BlockingFlow::Push(Edge* e)
{
	Node* from = e->LinkedEdge->TargetNode;
	Node* to = e->TargetNode;
	GRSuperNode* fromSuperNode = from->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	GRSuperNode* toSuperNode = to->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	GoldbergRao1998::GREdgeTag* edgeTag = e->GetTag<GoldbergRao1998::GREdgeTag*>();

	uint32_t delta = min(e->Capasity - edgeTag->newFlow, fromSuperNode->excess);
	edgeTag->newFlow += delta;
	fromSuperNode->excess -= delta;

	if(toSuperNode->excess == 0)	//The to node becomes active
	{
		m_queue->Enqueue(toSuperNode);
	}
	toSuperNode->excess += delta;
}

void BlockingFlow::Block(GRSuperNode* v)
{
	v->ResetToFirstEdge();

	if(m_blocked[v->id])
		return;

	GRSuperNode* childNode = m_forest->GetFirstChild(v);
	while(childNode != NULL)
	{
		GRSuperNode* nextSibling = m_forest->GetSibling(childNode);
		if(childNode->treeEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == v) //Only cut children that pushes
		{
			m_forest->Cut(childNode);
			Edge* cutEdge = childNode->treeEdge;
			cutEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = cutEdge->Capasity - m_forest->GetCost(childNode);
		}
		childNode = nextSibling;
	}			
	bool shouldBlock = true;

	if(!m_blocked[v->id])
	{
		while(v->m_currentEdge != NULL)
		{
			Edge* e = v->m_currentEdge;
			GRSuperNode* endPointSuperNode = e->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode;

			if(endPointSuperNode == v)
			{
				v->GetNextEdge();
				continue;
			}

			if(endPointSuperNode == m_sourceSuperNode)
			{
				v->GetNextEdge();
				continue;
			}

			uint32_t residualCapacity = e->Capasity - e->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow;
			if( residualCapacity == 0 || m_blocked[endPointSuperNode->id] || v->distance != endPointSuperNode->distance + EdgeLength(e) || endPointSuperNode->distance == UINT32_MAX)
			{
				v->GetNextEdge();
				continue;
			}
			shouldBlock = false;
			break;
		}

		if(shouldBlock)
			m_blocked[v->id] = true;
		v->ResetToFirstEdge();
	}
}

void BlockingFlow::SendPush( GRSuperNode* v)
{

	while(m_forest->GetRoot(v) != v && v->excess > 0)
	{
		GRSuperNode* cutNode = m_forest->GetMinBoundingNode(v);
		int delta = UtilityFunctions::min(v->excess,m_forest->GetCost(cutNode));
		m_forest->AddCost(v, -delta);
		v->excess -= delta;

		GRSuperNode* rootSuperNode = m_forest->GetRoot(v);

		if(rootSuperNode->excess == 0 && delta > 0)
			m_queue->Enqueue(rootSuperNode);
		rootSuperNode->excess += delta;
		
		while(m_forest->GetCost(cutNode) == 0 && m_forest->GetLink(cutNode) != NULL)
		{	
			Edge* cutEdge = cutNode->treeEdge;
			if(cutEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == cutNode) //Pull edge
				cutEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = 0;
			else
				cutEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = cutEdge->Capasity;
			m_forest->Cut(cutNode);
			if(m_forest->GetLink(v) == NULL)
				break;
			cutNode = m_forest->GetMinBoundingNode(v);
		}
	}
}

void BlockingFlow::SendPull( GRSuperNode* v)
{

	while(m_forest->GetRoot(v) != v && v->excess > 0)
	{
		GRSuperNode* cutNode = m_forest->GetMinBoundingNode(v);
		int delta = UtilityFunctions::min(v->excess,m_forest->GetCost(cutNode));
		m_forest->AddCost(v, -delta);
		v->excess -= delta;

		GRSuperNode* rootSuperNode = m_forest->GetRoot(v);

		if(rootSuperNode->excess == 0 && delta > 0)
			m_queue->Enqueue(rootSuperNode);
		rootSuperNode->excess += delta;
		
		while(m_forest->GetCost(cutNode) == 0 && m_forest->GetLink(cutNode) != NULL)
		{
			Edge* cutEdge = cutNode->treeEdge;
			if(cutEdge->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode == cutNode) //Pull edge
				cutEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = 0;
			else
				cutEdge->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow = cutEdge->Capasity;
			m_forest->Cut(cutNode);
			if(m_forest->GetLink(v) == NULL)
				break;
			cutNode = m_forest->GetMinBoundingNode(v);
		}
	}
}

bool BlockingFlow::TestIsBlockingFlow(Node* source, Node* target)
{
	GRSuperNode* sourceSuperNode = source->GetTag<GoldbergRao1998::GRTag*>()->superNode;
	GRSuperNode* targetSuperNode = target->GetTag<GoldbergRao1998::GRTag*>()->superNode;

	if(sourceSuperNode == targetSuperNode)
	{
		Debug << "Source is in the same superNode as sink\n";
		return true;
	}

	Queue<GRSuperNode*>* queue = m_queue;
	bool* seenBefore = m_blocked;

	for(bool* b = seenBefore; b < seenBefore + m_graph->NodeCount; b++)
	{
		*b = false;
	}

	queue->Enqueue(sourceSuperNode);
	seenBefore[sourceSuperNode->id] = true;

	while(queue->GetSize() > 0)
	{
		GRSuperNode* currentNode = queue->Dequeue();

		if(currentNode != sourceSuperNode && currentNode != targetSuperNode && currentNode->excess != 0)
		{
			Debug << "Error in found blocking flow, excess is not 0\n";
			return false;
		}

		currentNode->ResetToFirstEdge();
		while(currentNode->m_currentEdge != NULL)
		{
			Edge* e = currentNode->m_currentEdge;
			GRSuperNode* endPointSuperNode =e->TargetNode->GetTag<GoldbergRao1998::GRTag*>()->superNode;

			if(e->Capasity - e->GetTag<GoldbergRao1998::GREdgeTag*>()->newFlow == 0)
			{
				currentNode->GetNextEdge();
				continue;
			}

			if(currentNode->distance != endPointSuperNode->distance + EdgeLength(e))
			{
				currentNode->GetNextEdge();
				continue;
			}

			if(endPointSuperNode == m_sourceSuperNode)
			{
				currentNode->GetNextEdge();
				continue;
			}

			if(endPointSuperNode->id == targetSuperNode->id)
			{
				Debug << "Error in found blocking flow, sink is reachable\n";
				return false;
			}

			if(!seenBefore[endPointSuperNode->id])
			{
				queue->Enqueue(endPointSuperNode);
				seenBefore[endPointSuperNode->id] = true;
			}
			currentNode->GetNextEdge();
		}
	}
	return true;
}


#include <fstream>
#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */

#include <direct.h>
#include <sstream>
#define GetCurrentDir _getcwd

using namespace std;

void BlockingFlow::PrintGraph(Edge* activeEdge)
{
	if(!PRINT_IMAGES_BF)
		return;

	for(int i = 0; i < m_maxNodes; i++)
		nodesDrawed[i] = false;

	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");
	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node\n delta: " << m_delta << "\nColors:\norange: Same superNode\nblue: current edge\nred !admissible edge\nGreen node: current node\nFilled node: Blocked node\"";
	for (uint32_t i = 0; i < m_graph->NodeCount; i++)
	{
		GRSuperNode* superNode = m_graph->Nodes[i].GetTag<GoldbergRao1998::GRTag*>()->superNode;

		if( (!nodesDrawed[superNode->id] /*&& superNode->excess != 0*/) || superNode == m_sourceSuperNode || superNode == m_targetSuperNode)
		{
			nodesDrawed[superNode->id] = true;
			writer << "\nsubgraph cluster" << superNode->id << " {\n";
			writer << "\nlabel=\"Supernode #" << superNode->id << "\nDistance: " << superNode->distance << "\nExcess: " << superNode->excess << "\";";
			if(m_blocked[superNode->id])
				writer << "style = filled";
			for(int j = 0; j < superNode->m_subNodeCount; j++)
			{
				Node* node = superNode->m_subNodes[j];
				if(j != 0)
					writer << ';';

				GoldbergRao1998::GRTag* tag = node->GetTag<GoldbergRao1998::GRTag*>();
				writer << "\"Node: " << m_graph->IndexOf(node) + 1 << "\"[";
				if (activeEdge->LinkedEdge->TargetNode == node)
					writer << "color=green";
				writer << "]";
			}
			writer << "\n}";
		}
	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			if (edge->Capasity > 0)
			{

				GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
				GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();

				if( (!nodesDrawed[fromTag->superNode->id]) || (!nodesDrawed[toTag->superNode->id]))
					continue;

				writer << ";\"Node: " << node - m_graph->Nodes + 1;
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1;
				GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
				writer << "\"[label=\"" << edgeTag->newFlow << "/" << edge->Capasity << "\"";
				bool admissible = fromTag->distance == toTag->distance + EdgeLength(edge) && edge->Capasity - edgeTag->newFlow > 0;
				if(edgeTag->newFlow == 0 && HIDE_NO_FLOW_EDGES)
					writer << ", color=white, fontcolor=white, arrowhead=none, style=invis";
				else if(fromTag->superNode == toTag->superNode)
					writer << ", color=darkOrange";
				else if (edge == fromTag->superNode->m_currentEdge)
					writer << ", color=blue";
				else if (!admissible)
					writer << ", color=red";
				writer  << "]";
			}
		}
	}
	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\BFGraph" << m_runNrBF << "-" <<m_graphNrBF++ << ".jpg";

	int r = system(ss.str().c_str());
}