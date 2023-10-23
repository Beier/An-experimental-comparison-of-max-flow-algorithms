#include "GoldbergRao1998.h"
#include "../../Utilities/Debug.h"
#include <sstream>


using namespace std;

#define PRINT_IMAGES 0

Edge* GRSuperNode::GetNextEdge()
{
	if(m_currentEdge + 1 < m_subNodes[currentEdgeAt]->Edges + m_subNodes[currentEdgeAt]->EdgeCount)
	{
		return ++m_currentEdge;
	}

	if(currentEdgeAt == m_subNodeCount - 1) //Current edge is from the last node in the list
	{
		m_currentEdge = NULL;
		return m_currentEdge;
	}

	currentEdgeAt++;
	return m_currentEdge = m_subNodes[currentEdgeAt]->Edges;
}

bool GRSuperNode::IsLastEdge()
{
	return (currentEdgeAt == m_subNodeCount - 1 && m_currentEdge + 1 == m_subNodes[currentEdgeAt]->Edges + m_subNodes[currentEdgeAt]->EdgeCount);
}

bool GRSuperNode::HasPassedLastEdge()
{
	return (currentEdgeAt == m_subNodeCount - 1 && m_currentEdge + 1 == m_subNodes[currentEdgeAt]->Edges + m_subNodes[currentEdgeAt]->EdgeCount);
}

Edge* GRSuperNode::ResetToFirstEdge()
{
	currentEdgeAt = 0;
	return m_currentEdge = m_subNodes[currentEdgeAt]->Edges;
}

uint32_t GoldbergRao1998::CalculateMaxFlow(Graph* graph, Node* source, Node* target)
{
	m_theStack = new stack<uint32_t>();
	SCCs = new vector<vector<uint32_t>>();
	m_isOnStack = new bool[graph->NodeCount];

	m_blockingFlowCalculator = new BlockingFlow(graph->NodeCount);
	m_graph = graph;

	m_nodeTags = new GRTag[graph->NodeCount];
	m_edgeTags = new GREdgeTag[graph->EdgeCount];
	uint32_t edgeIndex = 0;
	for(Node* n = graph->Nodes; n < graph->Nodes + graph->NodeCount; n++)
	{
		int nodeNr = graph->IndexOf(n);
		n->Tag = &m_nodeTags[nodeNr];
		for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			e->Tag=&m_edgeTags[edgeIndex];
			edgeIndex++;
		}
	}	

	m_SuperNodes = new GRSuperNode[m_graph->NodeCount];
	m_subNodes = new Node*[m_graph->NodeCount];

	m_canonicalCutValues = new uint32_t[m_graph->NodeCount];
	m_translationMatrixOldToNewNode = new uint32_t[m_graph->NodeCount];
	m_recursionStack = new Node*[m_graph->NodeCount+1];
	m_routingTags = new GRRoutingTag[m_graph->NodeCount];
	m_InTreeQueue = new Queue<Node*>(m_graph->NodeCount);	
	m_OutTreeQueue = new Queue<Node*>(m_graph->NodeCount);

	m_lamda = min(sqrt(graph->EdgeCount), pow(graph->NodeCount, 2.0f/3.0f));
	F = graph->NodeCount * graph->GetMaxCapacity();
	m_delta = ceil(((float)F)/m_lamda);
	uint32_t flow = 0;
	uint32_t blockingFlow = 0;

	if(PRINT_IMAGES)
		m_graph->SaveJpg("images\\GraphBefore.jpg");

	Debug << "Initial values of F: " << F << " and m_delta: " << m_delta << "\n";

	runNr = 0;
	while(F >= 1)
	{
		Debug << "\nGenerating distances for run nr: "<< runNr << "\n";
		ResetTags();
		GenerateNodeDistances(source, target);

		if(source->GetTag<GRTag*>()->distance == UINT32_MAX)
			return flow;

		if(PRINT_IMAGES)
			PrintGraphWithNodes(source, target);

		Debug << "Finding SCCs\n";
		RunTarjan();
		m_nrSuperNodes = SCCs->size();
		Debug << "Contracting SCCs\n";
		ContractStronglyConnectedComponents(source, target);
		ResetSCCsEdges();
		if(source->GetTag<GRTag*>()->superNode != target->GetTag<GRTag*>()->superNode)
		{
			Debug << "Finding blocking flow\n";
			blockingFlow = m_blockingFlowCalculator->CalculateBlockingFlow(graph, m_SuperNodes, source, target, m_delta, SCCs->size(), runNr, true);
			Debug << "Found a blockingflow with value: " << blockingFlow  << "\n";

			if(blockingFlow == 0 && F <= 1)
				break;
			if(blockingFlow > m_delta)
			{
				uint32_t pushBackFlow = blockingFlow - m_delta;
				Debug << "Decreases blocking flow with "<< pushBackFlow << " units of flow\n";
				ResetSCCsEdges();
				DecreaseFlow(source, target, pushBackFlow);
			}
			Debug << "Extends to original graph\n";
			ResetSCCsEdges();
		}

		if(PRINT_IMAGES)
		{
			std::stringstream  ss1;
			ss1 << "images\\GraphAfterRun" << runNr << "ButBeforeExtending.jpg";
			m_graph->SaveJpg(ss1.str().c_str());
		}

		ExtendFlowToOriginalGraph(source, target);
		flow += m_routingTags[m_graph->IndexOf(target)].supply;

		if(PRINT_IMAGES)
		{
			std::stringstream  ss;
			ss << "images\\GraphAfterRun" << runNr << ".jpg";
			m_graph->SaveJpg(ss.str().c_str());
		}

		if(source->GetTag<GRTag*>()->superNode != target->GetTag<GRTag*>()->superNode)
		{
			Debug << "Finds the minimum Canonical cut\n";
			uint32_t minCanonicalCut = FindMinimumCanonicalCut(source, target);
			Debug << "Found a minimum Canonical cut\n with value: " << minCanonicalCut << "\n";

			if(minCanonicalCut <= F >> 1 || minCanonicalCut == 0)
			{
				F = minCanonicalCut;
				m_delta = ceil(((float)F)/m_lamda);
				Debug << "Updated F to: " << F << " and m_delta to: " << m_delta << "\n";
			}
		}
		else
			Debug << "Source and target is in the same superNode, so are not calculating the min. cut.";
		runNr++;
	}
	if(PRINT_IMAGES)
		m_graph->SaveJpg("images\\GraphAfter.jpg");
	
	delete[] m_isOnStack;
	delete m_theStack;
	delete SCCs;
	delete m_blockingFlowCalculator;
	delete[] m_nodeTags;
	delete[] m_edgeTags;
	delete[] m_SuperNodes;
	delete[] m_subNodes;
	delete[] m_canonicalCutValues;
	delete[] m_translationMatrixOldToNewNode;
	delete[] m_routingTags;
	delete[] m_recursionStack;
	delete m_InTreeQueue;
	delete m_OutTreeQueue;
	
	LinkedListNodeManager::ClearCache();
	return flow;
}

void GoldbergRao1998::ResetSCCsEdges()
{
	for(int i = 0; i < SCCs->size(); i++)
		m_SuperNodes[i].ResetToFirstEdge();
}

void GoldbergRao1998::GenerateNodeDistances( Node* source, Node* target)
{
	Queue<Node*>* treatCurrently = m_OutTreeQueue;
	Queue<Node*>* treatNext = m_InTreeQueue;


	target->GetTag<GRTag*>()->distance = 0;
	treatCurrently->Enqueue(target);

	while(treatCurrently->GetSize() > 0)
	{
		while (treatCurrently->GetSize() > 0)
		{
			Node* currentNode = treatCurrently->Dequeue();
			GRTag* tag = currentNode->GetTag<GRTag*>();

			Edge* edge = currentNode->Edges;
			for (uint32_t i = 0; i < currentNode->EdgeCount; i++)
			{

				Node* targetNode = edge->TargetNode;
				GRTag* targetTag = targetNode->GetTag<GRTag*>();
				Edge* linkedEdge = edge->LinkedEdge;

				if (linkedEdge->Capasity > 0)
				{
					if(IsZeroEdge(linkedEdge))
					{
						if(targetTag->distance > tag->distance)
						{
							targetTag->distance = tag->distance;
							treatCurrently->Enqueue(targetNode);
						}
					}
					else
					{
						if(targetTag->distance > tag->distance + 1)
						{
							targetTag->distance = tag->distance + 1 ;
							treatNext->Enqueue(targetNode);
						}
					}
				}
				edge++;
			}
		}
		Queue<Node*>* temp = treatCurrently;
		treatCurrently = treatNext;
		treatNext = temp;
	}
}

void GoldbergRao1998::ResetTags()
 {
	 for(int i = 0; i < m_graph->NodeCount; i++)
	 {
		 m_nodeTags[i].distance = UINT32_MAX;
		 m_nodeTags[i].superNode = NULL;
	 }
	 for(int i = 0; i < m_graph->EdgeCount; i++)
	 {
		 m_edgeTags[i].newFlow = 0;
	 }
 }
inline bool GoldbergRao1998::IsLargeEdge(Edge* e)
{
	return e->Capasity >= 2*m_delta;
}

inline bool GoldbergRao1998::IsSpecialEdge(Edge* e)
{
	
	if (e->Capasity >= m_delta && e->Capasity < 2*m_delta)
	{
		int distanceDifference = e->TargetNode->GetTag<GRTag*>()->distance - e->LinkedEdge->TargetNode->GetTag<GRTag*>()->distance;
		if(e->LinkedEdge->Capasity >= 2*m_delta && distanceDifference == 0)
			return true;
	}
	return false;
}

inline bool GoldbergRao1998::IsZeroEdge(Edge* e)
{
	return IsLargeEdge(e) || IsSpecialEdge(e);
}

uint32_t* GoldbergRao1998::GetDistances()
{
	uint32_t* resultingArray = new uint32_t[m_graph->NodeCount];
	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		int nodeNr = m_graph->IndexOf(n);
		resultingArray[nodeNr] = n->GetTag<GRTag*>()->distance;
	}
	return resultingArray;
}

uint32_t GoldbergRao1998::FindMinimumCanonicalCut(Node* source, Node* target)
{
	uint32_t sourceDistance = source->GetTag<GRTag*>()->superNode->distance;
	uint32_t targetDistance = target->GetTag<GRTag*>()->superNode->distance;

	if(sourceDistance == UINT32_MAX) //Source unreachable from sink.
		return 0;

	if(sourceDistance == targetDistance)
		return F;
	
	for(uint32_t* ccV = m_canonicalCutValues; ccV < m_canonicalCutValues + m_graph->NodeCount; ccV++)
	{
		*ccV = 0;
	}

	uint32_t largestDistanceSeen = 0;

	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		uint32_t fromNodeDistance = n->GetTag<GRTag*>()->distance;
		for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			uint32_t toNodeDistance = e->TargetNode->GetTag<GRTag*>()->distance;
			if(fromNodeDistance > toNodeDistance && fromNodeDistance != UINT32_MAX)
			{
				m_canonicalCutValues[fromNodeDistance] += e->Capasity;
				largestDistanceSeen = max(largestDistanceSeen, fromNodeDistance);
			}
		}
	}

	uint32_t largestDistanceAllowed = min(largestDistanceSeen, sourceDistance);

	uint32_t smallestCanonicalCutValue = m_canonicalCutValues[1];
	for(uint32_t i = 2; i <= largestDistanceAllowed; i++)
	{
		smallestCanonicalCutValue = min(smallestCanonicalCutValue, m_canonicalCutValues[i]);
	}
	return smallestCanonicalCutValue;
}

void GoldbergRao1998::ContractStronglyConnectedComponents(Node* source, Node* target)
{
	uint32_t* translationMatrixOldToNewNode = m_translationMatrixOldToNewNode;
	for(int i = 0; i < SCCs->size(); i++)
	{
		vector<uint32_t> component = SCCs->at(i);
		for(int j = 0; j < component.size(); j++)
		{
			translationMatrixOldToNewNode[component.at(j)] = i;
		}
	}

	//Make sure the source and target are the first in each of their components.
	uint32_t sourceIndex = m_graph->IndexOf(source);
	uint32_t targetIndex = m_graph->IndexOf(target);
	uint32_t sourceComponentId = translationMatrixOldToNewNode[sourceIndex];
	uint32_t targetComponentId = translationMatrixOldToNewNode[targetIndex];

	uint32_t originalFirstId = SCCs->at(sourceComponentId).at(0);
	if(originalFirstId != sourceIndex)
	{
		for(int i = 0; i < SCCs->at(sourceComponentId).size(); i++)
		{
			if(SCCs->at(sourceComponentId).at(i) == sourceIndex)
			{
				SCCs->at(sourceComponentId).at(i) = originalFirstId;
				break;
			}
		}
		SCCs->at(sourceComponentId).at(0) = sourceIndex;
	}

	originalFirstId = SCCs->at(targetComponentId).at(0);
	if(originalFirstId != targetIndex)
	{
		for(int i = 0; i < SCCs->at(targetComponentId).size(); i++)
		{
			if(SCCs->at(targetComponentId).at(i) == targetIndex)
			{
				SCCs->at(targetComponentId).at(i) = originalFirstId;
				break;
			}
		}
		SCCs->at(targetComponentId).at(0) = targetIndex;
	}

	for(int i = 0; i < m_graph->NodeCount; i++)
	{
		m_graph->Nodes[i].GetTag<GRTag*>()->superNode = &m_SuperNodes[translationMatrixOldToNewNode[i]];
	}

	uint32_t componentStartPosition = 0;
	for(int i = 0; i < SCCs->size(); i++)
	{
		vector<uint32_t> component = SCCs->at(i);
		m_SuperNodes[i].m_subNodes = &m_subNodes[componentStartPosition];
		
		for(int j = 0; j < component.size(); j++)
		{
			m_SuperNodes[i].m_subNodes[j] = &m_graph->Nodes[component.at(j)];
		}
		m_SuperNodes[i].excess = 0;
		m_SuperNodes[i].m_subNodeCount = component.size();
		m_SuperNodes[i].distance = m_SuperNodes[i].m_subNodes[0]->GetTag<GoldbergRao1998::GRTag*>()->distance;
		m_SuperNodes[i].ResetToFirstEdge();
		m_SuperNodes[i].id = i;
		m_SuperNodes[i].addedToQueue = false;

		componentStartPosition += component.size();
	}
}

void GoldbergRao1998::GetStronglyConnectedComponents()
{
	//http://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
	uint32_t* index = m_canonicalCutValues;
	uint32_t* lowlink = m_translationMatrixOldToNewNode;
	uint32_t lowestUnusedIndex = 0;

	if(!m_theStack->empty())
		throw new std::exception("Stack is not empty");
	
	SCCs->clear();

	for(uint32_t i = 0; i < m_graph->NodeCount; i++)
	{
		m_isOnStack[i] = false;
		index[i] = UINT32_MAX;
		lowlink[i] = UINT32_MAX;
	}

	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		if(index[m_graph->IndexOf(n)] == UINT32_MAX)
		{
			StrongConnect(n, index, lowlink, &lowestUnusedIndex);
		}
	}
}

void GoldbergRao1998::StrongConnect(Node* v, uint32_t* index, uint32_t* lowlink, uint32_t* lowestUnusedIndex)
{
	int nodeIndex = m_graph->IndexOf(v);
	index[nodeIndex] = *lowestUnusedIndex;
	lowlink[nodeIndex] = *lowestUnusedIndex;
	(*lowestUnusedIndex)++;
	m_theStack->push(nodeIndex);
	m_isOnStack[nodeIndex] = true;

	for(Edge* e = v->Edges; e < v->Edges + v->EdgeCount; e++)
	{
		if(IsZeroEdge(e))
		{
			Node* targetNode = e->TargetNode;
			uint32_t targetIndex = m_graph->IndexOf(targetNode);
			if(index[targetIndex] == UINT32_MAX)
			{
				StrongConnect(targetNode, index, lowlink, lowestUnusedIndex);
				lowlink[nodeIndex] = min(lowlink[nodeIndex], lowlink[targetIndex]);
			}
			else if (m_isOnStack[targetIndex])
			{
				lowlink[nodeIndex] = min(lowlink[nodeIndex], index[targetIndex]);
			}
		}
	}

	if(lowlink[nodeIndex] == index[nodeIndex])
	{
		vector<uint32_t> component;
		uint32_t stackIndex;
		do
		{
			stackIndex = m_theStack->top();
			m_theStack->pop();
			component.push_back(stackIndex);
			m_isOnStack[stackIndex] = false;
		}
		while(stackIndex != nodeIndex);
		SCCs->push_back(component);
	}
}

void GoldbergRao1998::RunTarjan()
{
	uint32_t lowestUnusedIndex = 0;
	uint32_t* index = m_canonicalCutValues;
	uint32_t* lowlink = m_translationMatrixOldToNewNode;

	if(!m_theStack->empty())
		throw new std::exception("Stack is not empty");
	
	SCCs->clear();

	for(uint32_t i = 0; i < m_graph->NodeCount; i++)
	{
		m_isOnStack[i] = false;
		index[i] = UINT32_MAX;
	}

	for (Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++) 
	{
		uint32_t nodeIndex = m_graph->IndexOf(n);
		if (index[nodeIndex] == UINT32_MAX) 
		{
			TarjanIter(n, &lowestUnusedIndex);
		}
	}
}

void GoldbergRao1998::TarjanIter(Node* v, uint32_t* lowestUnusedIndex)
{
	uint32_t* index = m_canonicalCutValues;
	uint32_t* lowlink = m_translationMatrixOldToNewNode;
	uint32_t nodeIndex = m_graph->IndexOf(v);

	index[nodeIndex] = *lowestUnusedIndex;
	lowlink[nodeIndex] = *lowestUnusedIndex;
	(*lowestUnusedIndex)++;

	v->GetTag<GRTag*>()->currentEdge = 0;
	m_theStack->push(nodeIndex);
	v->GetTag<GRTag*>()->superNode = NULL;

	m_isOnStack[nodeIndex] = true;

    Node *last = v;
    while(true) {
		uint32_t* lastCurrentEdge = &last->GetTag<GRTag*>()->currentEdge;
		uint32_t lastIndex = m_graph->IndexOf(last);
		if(*lastCurrentEdge < last->EdgeCount) 
		{       //Equivalent to the check in the for-loop in the recursive version
			Edge* currentEdge = &last->Edges[*lastCurrentEdge];
			(*lastCurrentEdge)++;    //Equivalent to incrementing the iterator in the for-loop in the recursive version
			if(IsZeroEdge(currentEdge))
			{
				Node *w = currentEdge->TargetNode;
				uint32_t wIndex = m_graph->IndexOf(w);
				if(index[wIndex] == UINT32_MAX) {
					GRTag* wTag = w->GetTag<GRTag*>();
					wTag->superNode = (GRSuperNode*)last;
					wTag->currentEdge = 0;
					index[wIndex] = *lowestUnusedIndex;
					lowlink[wIndex] = *lowestUnusedIndex;
					(*lowestUnusedIndex)++;
					m_theStack->push(wIndex);
					m_isOnStack[wIndex] = true;
					last = w;
				}
				else if (m_isOnStack[wIndex])
				{
					lowlink[lastIndex] = min(lowlink[lastIndex], index[wIndex]);
				}
			}

        } else 
		{  //Equivalent to the nodeSet iterator pointing to end()
			if(lowlink[lastIndex] == index[lastIndex])
			{
				vector<uint32_t> component;
				uint32_t stackIndex;
				do
				{
					stackIndex = m_theStack->top();
					m_theStack->pop();
					component.push_back(stackIndex);
					m_isOnStack[stackIndex] = false;
				}
				while(stackIndex != lastIndex);
				SCCs->push_back(component);
			}

			Node *newLast = (Node*)last->GetTag<GRTag*>()->superNode;   //Go up one recursive call
            if(newLast != NULL) {
				uint32_t newLastIndex = m_graph->IndexOf(newLast);
				lowlink[newLastIndex] = min(lowlink[newLastIndex], lowlink[lastIndex]);
                last = newLast;
            } else {   //We've seen all the nodes
                break;
            }
        }
    }
}


void GoldbergRao1998::DecreaseFlow(Node* source, Node* target, uint32_t decreaseBy)
{
	GRSuperNode* targetSuperNode = target->GetTag<GRTag*>()->superNode;
	targetSuperNode->excess = decreaseBy;
	Queue<Node*>* nodeQueue = m_InTreeQueue;
	nodeQueue->Enqueue(target);

	Node* v;
	m_graphNrDF = 0;

	while((v = nodeQueue->Dequeue()) != NULL)
	{
		GRSuperNode* superNode = v->GetTag<GRTag*>()->superNode;
		if(PRINT_IMAGES)
			PrintDecreaseFlowGraph(superNode);

		if(superNode == source->GetTag<GRTag*>()->superNode)
			continue;

		uint32_t* excess = &superNode->excess;
		while(*excess != 0)
		{
			Edge* e = superNode->m_currentEdge;
			uint32_t* linkedEdgeFlow = &e->LinkedEdge->GetTag<GREdgeTag*>()->newFlow;
			if( *linkedEdgeFlow > 0)
			{
				uint32_t pushBackFlow = min(*excess, *linkedEdgeFlow);
				*linkedEdgeFlow -= pushBackFlow;
				*excess -= pushBackFlow;
				uint32_t* targetNodeExcess = &e->TargetNode->GetTag<GRTag*>()->superNode->excess;
				if( *targetNodeExcess == 0)
				{
					nodeQueue->Enqueue(e->TargetNode);
				}
				*targetNodeExcess += pushBackFlow;
			}
			superNode->GetNextEdge();
			if(superNode->m_currentEdge == NULL)
				superNode->ResetToFirstEdge();
		}
	}
	if(PRINT_IMAGES)
		PrintDecreaseFlowGraph(targetSuperNode);
}

void GoldbergRao1998::ExtendFlowToOriginalGraph(Node* source, Node* target) //, uint32_t* translationMatrixOldToNewNode
{
	//Reset routing tags
	for(int i = 0; i < m_graph->NodeCount;i++)
	{
		m_routingTags[i].InTreeChild = NULL;
		m_routingTags[i].InTreeParentEdge = NULL;
		m_routingTags[i].InTreeSibling = NULL;

		m_routingTags[i].OutTreeChild = NULL;
		m_routingTags[i].OutTreeParentEdge = NULL;
		m_routingTags[i].OutTreeSibling = NULL;

		m_routingTags[i].descendantDemand = UINT32_MAX;
		m_routingTags[i].demand = 0;
		m_routingTags[i].supply = 0;
		m_routingTags[i].visitedMoveSupply = false;
		m_routingTags[i].visitedMoveDemand = false;
	}

	bool* needsRouting = m_isOnStack;
	for(int i = 0; i < SCCs->size(); i++)
	{
		needsRouting[i] = false;
	}

	if(source->GetTag<GRTag*>()->superNode == target->GetTag<GRTag*>()->superNode)
	{
		m_routingTags[m_graph->IndexOf(source)].supply = m_delta;
		needsRouting[source->GetTag<GRTag*>()->superNode->id] = true;
	}
	else
	{
		//Set supply and demand
		for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
		{
			uint32_t supply = 0;
			uint32_t demand = 0;
			for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
			{
				demand += e->GetTag<GREdgeTag*>()->newFlow;
				supply += e->LinkedEdge->GetTag<GREdgeTag*>()->newFlow;
			}
			if(supply != 0 || demand != 0)
			{
				uint32_t nodeId = m_graph->IndexOf(n);
				needsRouting[n->GetTag<GRTag*>()->superNode->id] = true;
				m_routingTags[nodeId].demand = demand;
				m_routingTags[nodeId].supply = supply;
			}
		}
	}
	if(PRINT_IMAGES)
		PrintSupplyDemandGraph(source, target, "SupplyDemandBefore");

	for(int i = 0; i < SCCs->size(); i++)
	{
		if(!needsRouting[i])
			continue;

		uint32_t rootNodeIndex = SCCs->at(i).at(0);
		Node* rootNode = &m_graph->Nodes[rootNodeIndex];
		uint32_t componentId = rootNode->GetTag<GRTag*>()->superNode->id;
		GRRoutingTag* rootRoutingTag = &m_routingTags[rootNodeIndex];

		//Create in and out tree
		for(Edge* e = rootNode->Edges; e < rootNode->Edges + rootNode->EdgeCount; e++)
		{

			if(e->TargetNode->GetTag<GRTag*>()->superNode->id == componentId)
			{
				GRRoutingTag* toRoutingTag = &m_routingTags[m_graph->IndexOf(e->TargetNode)];
				if(IsZeroEdge(e))
				{
					LinkNodes(e->TargetNode, toRoutingTag, rootRoutingTag, e, false);
					m_OutTreeQueue->Enqueue(e->TargetNode);
				}
				if(IsZeroEdge(e->LinkedEdge))
				{
					LinkNodes(e->TargetNode, toRoutingTag, rootRoutingTag, e->LinkedEdge, true);
					m_InTreeQueue->Enqueue(e->TargetNode);
				}
			}
		}

		Node* v;
		while((v = m_InTreeQueue->Dequeue()) != NULL)
		{
			LinkInTree(v, i , rootNodeIndex);
		}
		while((v = m_OutTreeQueue->Dequeue()) != NULL)
		{
			LinkOutTree(v, i, rootNodeIndex);
		}

		if(PRINT_IMAGES)
		{
			PrintInTree(source, target, componentId);
			PrintOutTree(source, target, componentId);
		}

		CalcDescDemands(rootNodeIndex);

		MoveSupplyForward(rootNodeIndex);

		MoveDemandBackwards(rootNodeIndex);
	}

	if(PRINT_IMAGES)
		PrintSupplyDemandGraph(source, target, "SupplyDemandAfter");
	

	uint32_t edgeIndex = 0;
	for(Node* n = m_graph->Nodes; n < m_graph->Nodes + m_graph->NodeCount; n++)
	{
		for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
		{
			uint32_t newFlow = e->GetTag<GREdgeTag*>()->newFlow;
			e->Capasity -= newFlow;
			e->LinkedEdge->Capasity += newFlow;
			edgeIndex++;
		}
	}	
}

void GoldbergRao1998::LinkInTree(Node* v, uint32_t componentId, uint32_t rootNodeIndex)
{
	uint32_t nodeId = m_graph->IndexOf(v);
	for(Edge* e = v->Edges; e < v->Edges + v->EdgeCount; e++)
	{

		Node* targetNode = e->TargetNode;
		uint32_t targetNodeId = m_graph->IndexOf(targetNode);

		if(targetNodeId == rootNodeIndex)
			continue;

		if(m_routingTags[targetNodeId].InTreeParentEdge != NULL)
			continue;

		if(!IsZeroEdge(e->LinkedEdge))
			continue;

		if(targetNode->GetTag<GRTag*>()->superNode->id == componentId)
		{
			m_InTreeQueue->Enqueue(targetNode);
			LinkNodes(targetNode, &m_routingTags[targetNodeId], &m_routingTags[nodeId], e->LinkedEdge, true);
		}
	}
}

void GoldbergRao1998::LinkOutTree(Node* v, uint32_t componentId, uint32_t rootNodeIndex)
{
	uint32_t nodeId = m_graph->IndexOf(v);
	for(Edge* e = v->Edges; e < v->Edges + v->EdgeCount; e++)
	{

		Node* targetNode = e->TargetNode;
		uint32_t targetNodeId = m_graph->IndexOf(targetNode);

		if(targetNodeId == rootNodeIndex)
			continue;

		if(m_routingTags[targetNodeId].OutTreeParentEdge != NULL)
			continue;

		if(!IsZeroEdge(e))
			continue;

		if(targetNode->GetTag<GRTag*>()->superNode->id == componentId)
		{
			LinkNodes(targetNode, &m_routingTags[targetNodeId], &m_routingTags[nodeId], e, false);
			m_OutTreeQueue->Enqueue(targetNode);
		}
	}
}

void GoldbergRao1998::LinkNodes(Node* child, GRRoutingTag* childTag, GRRoutingTag* parentTag, Edge* parentEdge, bool Intree)
{
	if(Intree)
	{
		if(parentTag->InTreeChild != NULL)
			childTag->InTreeSibling = parentTag->InTreeChild;
		parentTag->InTreeChild = child;	
		childTag->InTreeParentEdge = parentEdge;
	}
	else
	{
		if(parentTag->OutTreeChild != NULL)
			childTag->OutTreeSibling = parentTag->OutTreeChild;
		parentTag->OutTreeChild = child;	
		childTag->OutTreeParentEdge = parentEdge;
	}
}

uint32_t GoldbergRao1998::CalcDescDemands(uint32_t rootId)
{
	uint32_t stackIndex = 0;
	m_recursionStack[++stackIndex] = &m_graph->Nodes[rootId];
	while(stackIndex > 0)
	{
		Node* node = m_recursionStack[stackIndex];
		uint32_t nodeId = m_graph->IndexOf(node);
		GRRoutingTag* nodeRoutingTag = &m_routingTags[nodeId];
		if(nodeRoutingTag->descendantDemand == UINT32_MAX && nodeRoutingTag->OutTreeChild != NULL) //First time we visit a node
		{
			Node* child = nodeRoutingTag->OutTreeChild;
			while(child != NULL)
			{
				uint32_t childId = m_graph->IndexOf(child);
				m_recursionStack[++stackIndex] = child;
				child = m_routingTags[childId].OutTreeSibling;
			}
		} else
		{
			if(nodeRoutingTag->OutTreeChild==NULL)
				nodeRoutingTag->descendantDemand = nodeRoutingTag->demand;
			else
				nodeRoutingTag->descendantDemand += nodeRoutingTag->demand;
			if(nodeRoutingTag->OutTreeParentEdge != NULL)
			{
				GRRoutingTag* parentRoutingTag = &m_routingTags[ m_graph->IndexOf(nodeRoutingTag->OutTreeParentEdge->LinkedEdge->TargetNode)];
				if(parentRoutingTag->descendantDemand == UINT32_MAX)
					parentRoutingTag->descendantDemand = nodeRoutingTag->descendantDemand;
				else
					parentRoutingTag->descendantDemand += nodeRoutingTag->descendantDemand;
			}
			stackIndex--;
		}
	}
}

void GoldbergRao1998::MoveSupplyForward(uint32_t rootId)
{
	uint32_t stackIndex = 0;
	m_recursionStack[++stackIndex] = &m_graph->Nodes[rootId];
	GRRoutingTag* nodeRoutingTag = &m_routingTags[rootId];
	Node* child = nodeRoutingTag->InTreeChild;
	while(child != NULL)
	{
		uint32_t childId = m_graph->IndexOf(child);
		m_recursionStack[++stackIndex] = child;
		child = m_routingTags[childId].InTreeSibling;
	}

	while(stackIndex > 1)
	{
		Node* node = m_recursionStack[stackIndex];
		uint32_t nodeId = m_graph->IndexOf(node);
		GRRoutingTag* nodeRoutingTag = &m_routingTags[nodeId];
		if(!nodeRoutingTag->visitedMoveSupply && nodeRoutingTag->InTreeChild != NULL) //First time we visit a node
		{
			Node* child = nodeRoutingTag->InTreeChild;
			while(child != NULL)
			{
				uint32_t childId = m_graph->IndexOf(child);
				m_recursionStack[++stackIndex] = child;
				child = m_routingTags[childId].InTreeSibling;
			}
		} else
		{
			uint32_t supplyMoved = min(nodeRoutingTag->supply, m_delta - nodeRoutingTag->descendantDemand);
			nodeRoutingTag->supply -= supplyMoved;
			nodeRoutingTag->InTreeParentEdge->GetTag<GREdgeTag*>()->newFlow += supplyMoved;
			m_routingTags[m_graph->IndexOf(nodeRoutingTag->InTreeParentEdge->TargetNode)].supply += supplyMoved;
			stackIndex--;
		}
		nodeRoutingTag->visitedMoveSupply = true;
	}
}

void GoldbergRao1998::MoveDemandBackwards(uint32_t rootId)
{
	uint32_t stackIndex = 0;
	m_recursionStack[++stackIndex] = &m_graph->Nodes[rootId];
	GRRoutingTag* nodeRoutingTag = &m_routingTags[rootId];
	Node* child = nodeRoutingTag->OutTreeChild;
	while(child != NULL)
	{
		uint32_t childId = m_graph->IndexOf(child);
		m_recursionStack[++stackIndex] = child;
		child = m_routingTags[childId].OutTreeSibling;
	}

	while(stackIndex > 1)
	{
		Node* node = m_recursionStack[stackIndex];
		uint32_t nodeId = m_graph->IndexOf(node);
		GRRoutingTag* nodeRoutingTag = &m_routingTags[nodeId];
		if(!nodeRoutingTag->visitedMoveDemand && nodeRoutingTag->OutTreeChild != NULL) //First time we visit a node
		{
			Node* child = nodeRoutingTag->OutTreeChild;
			while(child != NULL)
			{
				uint32_t childId = m_graph->IndexOf(child);
				m_recursionStack[++stackIndex] = child;
				child = m_routingTags[childId].OutTreeSibling;
			}
		} else
		{
			uint32_t demandMoved = nodeRoutingTag->demand - nodeRoutingTag->supply;
			nodeRoutingTag->OutTreeParentEdge->GetTag<GREdgeTag*>()->newFlow += demandMoved;
			m_routingTags[m_graph->IndexOf(nodeRoutingTag->OutTreeParentEdge->LinkedEdge->TargetNode)].demand += demandMoved;	
			stackIndex--;
		}
		nodeRoutingTag->visitedMoveDemand = true;
	}
}

bool GoldbergRao1998::CheckAllEdgeCapacities()
{
	for(int i = 0; i < m_graph->NodeCount; i++)
	{
		Node* originalNode = &m_graphClone->Nodes[i];
		Node* manipulatedNode = &m_graph->Nodes[i];
		for(int j = 0; j < originalNode->EdgeCount; j++)
		{
			Edge* originalEdge = &originalNode->Edges[j];
			Edge* manipulatedEdge = &manipulatedNode->Edges[j];
			if(originalEdge->Capasity + originalEdge->LinkedEdge->Capasity != manipulatedEdge->Capasity + manipulatedEdge->LinkedEdge->Capasity)
				return false;
		}
	}
	return true;
}

Node* GoldbergRao1998::CheckFlowConservation(Node* source, Node* target)
{
	for(int i = 0; i < m_graph->NodeCount; i++)
	{
		Node* originalNode = &m_graphClone->Nodes[i];
		Node* manipulatedNode = &m_graph->Nodes[i];
		uint32_t ingoingFlow = 0;
		uint32_t outgoingFlow = 0;

		for(int j = 0; j < originalNode->EdgeCount; j++)
		{
			Edge* originalEdge = &originalNode->Edges[j];
			Edge* manipulatedEdge = &manipulatedNode->Edges[j];
			if(originalEdge->Capasity > manipulatedEdge->Capasity)
			{
				outgoingFlow += originalEdge->Capasity - manipulatedEdge->Capasity;
			}
			else
			{
				ingoingFlow += originalEdge->LinkedEdge->Capasity - manipulatedEdge->LinkedEdge->Capasity;
			}
		}
		if(manipulatedNode != source && manipulatedNode != target && ingoingFlow != outgoingFlow)
			return manipulatedNode;
	}
	return NULL;
}

bool GoldbergRao1998::CheckNewFlowForNode(Node* node)
{
	uint32_t outgoingFlow = 0;
	uint32_t ingoingFlow = 0;

	for(Edge* e = node->Edges; e < node->Edges + node->EdgeCount; e++)
	{
		outgoingFlow += e->GetTag<GREdgeTag*>()->newFlow;
		ingoingFlow += e->LinkedEdge->GetTag<GREdgeTag*>()->newFlow;
	}
	if(outgoingFlow != ingoingFlow)
		return false;
	return true;
}


#include <fstream>
#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */

#include <direct.h>
#define GetCurrentDir _getcwd

using namespace std;

void GoldbergRao1998::PrintGraphWithNodes(Node* source, Node* target)
{
	if(!PRINT_IMAGES)
		return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");
	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node\n delta: " << m_delta << "\nColors:\norange: Large Edge\nblue: special edge\ngreen admissible edge\"";
	for (uint32_t i = 0; i < m_graph->NodeCount; i++)
	{
		GoldbergRao1998::GRTag* tag = m_graph->Nodes[i].GetTag<GoldbergRao1998::GRTag*>();
		writer << "\"Node: " << i + 1 << "\n distance:" << tag->distance << "\"\n[";
		if (&m_graph->Nodes[i] == source)
			writer << "color=green";		
		if (&m_graph->Nodes[i] == target)
			writer << "color=red";
		writer << "]";
	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
			//if (edge->Capasity > 0)
			{
				GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
				writer << ";\"Node: " << node - m_graph->Nodes + 1  << "\n distance:" << fromTag->distance;
				GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1  << "\n distance:" << toTag->distance;
				writer << "\"[label=\"" << edge->Capasity << "\"";
				if(edge->Capasity == 0)
					writer << ", color=white, arrowhead=none, fontcolor = white";
				else if(IsZeroEdge(edge))
					writer << ", color=darkorange";
				//else if(IsZeroEdge(edge))
				//	writer << ", color=blue";
				else if (fromTag->distance == toTag->distance + !IsZeroEdge(edge))
					writer << ", color=green";
				writer  << "]";
			}
		}
	}
	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\DistanceGraph" << runNr << ".jpg";

	int r = system(ss.str().c_str());
}

void GoldbergRao1998::PrintDecreaseFlowGraph(GRSuperNode* currentSuperNode)
{
	if(!PRINT_IMAGES)
		return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");

	writer.open(tempFile);
	writer << "digraph G {";

	for(int i = 0; i < m_nrSuperNodes; i++)
	{
		writer << "\nsubgraph cluster" << i << " {\n";
		
		GRSuperNode* superNode = &m_SuperNodes[i];
		writer << "\nlabel=\"Supernode #" << i << "\nDistance: " << superNode->distance << "\nExcess: " << superNode->excess << "\";";
		for(int j = 0; j < superNode->m_subNodeCount; j++)
		{
			Node* node = superNode->m_subNodes[j];
			if(j != 0)
				writer << ';';

			GoldbergRao1998::GRTag* tag = node->GetTag<GoldbergRao1998::GRTag*>();
			writer << "\"Node: " << m_graph->IndexOf(node) + 1 << "\"[";
			if (tag->superNode == currentSuperNode)
				writer << "color=green";
			writer << "]";
		}
		writer << ";\n}";
	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
			GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
			GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();
			if (edgeTag->newFlow > 0 || (fromTag->superNode == toTag->superNode && IsLargeEdge(edge)))
			{
				writer << ";\"Node: " << node - m_graph->Nodes + 1;
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1;
				writer << "\"[label=\"" << edgeTag->newFlow << "/" << edge->Capasity << "\"";
				if (edge == fromTag->superNode->m_currentEdge)
					writer << ", color=blue";
				if(fromTag->superNode == toTag->superNode)
					writer << ", color=darkorange";
				writer  << "]";
			}
		}
	}

	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\DFGraph" << 100 * runNr + m_graphNrDF++ << ".jpg";

	int r = system(ss.str().c_str());
}

void GoldbergRao1998::PrintSupplyDemandGraph(Node* source, Node* target, const char* fileName)
{
	if(!PRINT_IMAGES)
		return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");

	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node\n delta: " << m_delta << "\nColors:\ngreen node: Source\nRed node: sink\nFilled node: root of SCC\"";

	for(int i = 0; i < m_nrSuperNodes; i++)
	{
		writer << "\nsubgraph cluster" << i << " {\n";
		
		GRSuperNode* superNode = &m_SuperNodes[i];
		writer << "\nlabel=\"Supernode #" << i << "\nDistance: " << superNode->distance << "\nExcess: " << superNode->excess << "\";";
		for(int j = 0; j < superNode->m_subNodeCount; j++)
		{
			Node* node = superNode->m_subNodes[j];
			uint32_t nodeId = m_graph->IndexOf(node);
			if(j != 0)
				writer << ';';

			GoldbergRao1998::GRTag* tag = node->GetTag<GoldbergRao1998::GRTag*>();
			writer << "\"Node: " << m_graph->IndexOf(node) + 1 << "\"[";
			writer << "label=\"Node: " << m_graph->IndexOf(node) + 1 <<"\nSupply: " << m_routingTags[nodeId].supply << "\nDemand: " << m_routingTags[nodeId].demand << "\"";
			if(node == source)
				writer << ", color=green";
			if(node == target)
				writer << ", color=red";
			if(SCCs->at(node->GetTag<GRTag*>()->superNode->id).at(0) == nodeId) //Root in the SCC
				writer << ", style=filled";
			writer << "]";
		}
		writer << ";\n}";


	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
			GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
			GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();
			if (edgeTag->newFlow > 0 || (fromTag->superNode == toTag->superNode && IsZeroEdge(edge)))
			{
				writer << ";\"Node: " << node - m_graph->Nodes + 1;
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1;
				writer << "\"[label=\"" << edgeTag->newFlow << "/" << edge->Capasity << "\"";
				if(fromTag->superNode == toTag->superNode)
					writer << ", color=darkorange";
				writer  << "]";
			}
		}
	}

	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\"<< fileName << runNr << ".jpg";

	int r = system(ss.str().c_str());
}

void GoldbergRao1998::PrintInTree(Node* source, Node* target, uint32_t SccId)
{
	if(!PRINT_IMAGES)
		return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");

	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node\n delta: " << m_delta << "\nColors:\ngreen node: Source\nRed node: sink\nFilled node: root of \nGreen edge: sibling in In-tree\nOrange edge: parent edge in In-tree \"";

	for(int i = 0; i < m_nrSuperNodes; i++)
	{
		writer << "\nsubgraph cluster" << i << " {\n";
		
		GRSuperNode* superNode = &m_SuperNodes[i];
		writer << "\nlabel=\"Supernode #" << i << "\nDistance: " << superNode->distance << "\nExcess: " << superNode->excess << "\";";
		for(int j = 0; j < superNode->m_subNodeCount; j++)
		{
			Node* node = superNode->m_subNodes[j];
			uint32_t nodeId = m_graph->IndexOf(node);
			if(j != 0)
				writer << ';';

			GoldbergRao1998::GRTag* tag = node->GetTag<GoldbergRao1998::GRTag*>();
			writer << "\"Node: " << m_graph->IndexOf(node) + 1 << "\"[";
			writer << "label=\"Node: " << m_graph->IndexOf(node) + 1 <<"\nSupply: " << m_routingTags[nodeId].supply << "\nDemand: " << m_routingTags[nodeId].demand << "\"";
			if(node == source)
				writer << ", color=green";
			if(node == target)
				writer << ", color=red";
			if(SCCs->at(node->GetTag<GRTag*>()->superNode->id).at(0) == nodeId) //Root in the SCC
				writer << ", style=filled";
			writer << "]";
		}
		writer << ";\n}";


	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
			GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
			GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();
			if (edge->Capasity > 0 || edgeTag->newFlow > 0 || (fromTag->superNode == toTag->superNode && IsZeroEdge(edge)) || (m_routingTags[m_graph->IndexOf(node)].InTreeSibling == edge->TargetNode))
			{
				writer << ";\"Node: " << node - m_graph->Nodes + 1;
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1;
				writer << "\"[label=\"" << edgeTag->newFlow << "/" << edge->Capasity << "\"";
				if(m_routingTags[m_graph->IndexOf(node)].InTreeParentEdge == edge)
					writer << ", color=darkorange";
				else if (m_routingTags[m_graph->IndexOf(node)].InTreeSibling == edge->TargetNode)
					writer << ", color=green";
				writer  << "]";
			}
		}
	}

	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\IntreeGraph" << runNr << "-" << SccId << ".jpg";

	int r = system(ss.str().c_str());
}

void GoldbergRao1998::PrintOutTree(Node* source, Node* target, uint32_t SccId)
{
	if(!PRINT_IMAGES)
		return;
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");

	writer.open(tempFile);
	writer << "digraph G {";
	writer << "\"Node: Description Node\n delta: " << m_delta << "\nColors:\ngreen node: Source\nRed node: sink\nFilled node: root of \nGreen edge: sibling in Out-tree\nOrange edge: parent edge in Out-tree \"";

	for(int i = 0; i < m_nrSuperNodes; i++)
	{
		writer << "\nsubgraph cluster" << i << " {\n";
		
		GRSuperNode* superNode = &m_SuperNodes[i];
		writer << "\nlabel=\"Supernode #" << i << "\nDistance: " << superNode->distance << "\nExcess: " << superNode->excess << "\";";
		for(int j = 0; j < superNode->m_subNodeCount; j++)
		{
			Node* node = superNode->m_subNodes[j];
			uint32_t nodeId = m_graph->IndexOf(node);
			if(j != 0)
				writer << ';';

			GoldbergRao1998::GRTag* tag = node->GetTag<GoldbergRao1998::GRTag*>();
			writer << "\"Node: " << m_graph->IndexOf(node) + 1 << "\"[";
			writer << "label=\"Node: " << m_graph->IndexOf(node) + 1 << "\nSupply: " << m_routingTags[nodeId].supply << "\nDemand: " << m_routingTags[nodeId].demand << "\"";
			if(node == source)
				writer << ", color=green";
			if(node == target)
				writer << ", color=red";
			if(SCCs->at(node->GetTag<GRTag*>()->superNode->id).at(0) == nodeId) //Root in the SCC
				writer << ", style=filled";
			writer << "]";
		}
		writer << ";\n}";


	}

	for (Node* node = m_graph->Nodes; node < m_graph->Nodes + m_graph->NodeCount; node++)
	{
		for (Edge* edge = node->Edges; edge < node->Edges + node->EdgeCount; edge++)
		{
			GoldbergRao1998::GREdgeTag* edgeTag = edge->GetTag<GoldbergRao1998::GREdgeTag*>();
			GoldbergRao1998::GRTag* fromTag = node->GetTag<GoldbergRao1998::GRTag*>();
			GoldbergRao1998::GRTag* toTag = edge->TargetNode->GetTag<GoldbergRao1998::GRTag*>();
			if (edge->Capasity > 0 || edgeTag->newFlow > 0 || (fromTag->superNode == toTag->superNode && IsZeroEdge(edge)) || (m_routingTags[m_graph->IndexOf(node)].OutTreeSibling == edge->TargetNode))
			{
				writer << ";\"Node: " << node - m_graph->Nodes + 1;
				writer << "\"->\"Node: " << edge->TargetNode - m_graph->Nodes + 1;
				writer << "\"[label=\"" << edgeTag->newFlow << "/" << edge->Capasity << "\"";
				if(m_routingTags[m_graph->IndexOf(edge->TargetNode)].OutTreeParentEdge== edge)
					writer << ", color=darkorange";
				else if (m_routingTags[m_graph->IndexOf(node)].OutTreeSibling == edge->TargetNode)
					writer << ", color=green";
				writer  << "]";
			}
		}
	}

	writer<<'}';
	writer.flush();
	writer.close();
	
	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o images\\OuttreeGraph" << runNr << "-" << SccId << ".jpg";

	int r = system(ss.str().c_str());
}



//Graph* GoldbergRao1998::ContractStronglyConnectedComponents(Node* source, Node* target)
//{
//	vector<vector<uint32_t>>* stronglyConnectedComponents = GetStronglyConnectedComponents();
//
//	uint32_t* translationMatrixOldToNewNode = new uint32_t[m_graph->NodeCount];
//	for(int i = 0; i < stronglyConnectedComponents->size(); i++)
//	{
//		vector<uint32_t> component = stronglyConnectedComponents->at(i);
//		for(int j = 0; j < component.size(); j++)
//		{
//			translationMatrixOldToNewNode[component.at(j)] = i;
//		}
//	}
//
//	//Make sure the source and target are the first in each of their components.
//	uint32_t sourceIndex = m_graph->IndexOf(source);
//	uint32_t targetIndex = m_graph->IndexOf(target);
//	uint32_t sourceComponentId = translationMatrixOldToNewNode[sourceIndex];
//	uint32_t targetComponentId = translationMatrixOldToNewNode[m_graph->IndexOf(target)];
//
//	uint32_t originalFirstId = stronglyConnectedComponents->at(sourceComponentId).at(0);
//	if(originalFirstId != sourceIndex)
//	{
//		for(int i = 0; i < stronglyConnectedComponents->at(sourceComponentId).size(); i++)
//		{
//			if(stronglyConnectedComponents->at(sourceComponentId).at(i) == sourceIndex)
//			{
//				stronglyConnectedComponents->at(sourceComponentId).at(i) = originalFirstId;
//				break;
//			}
//		}
//		stronglyConnectedComponents->at(sourceComponentId).at(0) = sourceIndex;
//	}
//
//	originalFirstId = stronglyConnectedComponents->at(targetComponentId).at(0);
//	if(originalFirstId != targetIndex)
//	{
//		for(int i = 0; i < stronglyConnectedComponents->at(targetComponentId).size(); i++)
//		{
//			if(stronglyConnectedComponents->at(targetComponentId).at(i) == targetIndex)
//			{
//				stronglyConnectedComponents->at(targetComponentId).at(i) = originalFirstId;
//				break;
//			}
//		}
//		stronglyConnectedComponents->at(targetComponentId).at(0) = targetIndex;
//	}
//
//
//	Node* newNodes = new Node[stronglyConnectedComponents->size()];
//
//	uint32_t* nrEdgesPrComponent = CalcNrSuperEdges(stronglyConnectedComponents, translationMatrixOldToNewNode);
//
//	uint32_t* edgeCapacities = new uint32_t[stronglyConnectedComponents->size()];
//	vector<uint32_t> endNodes;
//	vector<Edge*>* oldEdgePointer = new vector<Edge*>[stronglyConnectedComponents->size()];
//
//	for(int i = 0; i < stronglyConnectedComponents->size(); i++)
//	{
//		edgeCapacities[i] = UINT32_MAX;
//		oldEdgePointer[i] = vector<Edge*>();
//	}
//
//	for(int i = 0; i < stronglyConnectedComponents->size(); i++)
//	{
//		endNodes.clear();
//		vector<uint32_t> component = stronglyConnectedComponents->at(i);
//		Edge* newEdges = new Edge[nrEdgesPrComponent[i]];
//		
//
//		for(int j = 0; j < component.size(); j++)
//		{
//			uint32_t nodeIndex = component.at(j);
//			Node* node = &m_graph->Nodes[nodeIndex];
//			uint32_t nodeDistance = node->GetTag<GRTag*>()->distance;
//
//			for(Edge* e = node->Edges; e < node->Edges + node->EdgeCount; e++)
//			{
//				uint32_t targetNodeIndex = m_graph->IndexOf(e->TargetNode);
//				uint32_t newNodeIndex = translationMatrixOldToNewNode[targetNodeIndex];
//
//				if(newNodeIndex == i) //The edge goes to an internal node in the new superNode
//				{
//					continue;
//				}
//
//				if(nodeDistance == e->TargetNode->GetTag<GRTag*>()->distance + 1 || e->Capasity > 3 * m_delta) //If an admissible edge
//				{
//					if(edgeCapacities[newNodeIndex] == UINT32_MAX)
//					{
//						oldEdgePointer[newNodeIndex].clear();
//						oldEdgePointer[newNodeIndex].push_back(e);
//						endNodes.push_back(newNodeIndex);
//						edgeCapacities[newNodeIndex] = e->Capasity;
//
//					}
//					else
//					{
//						edgeCapacities[newNodeIndex] += e->Capasity;
//						oldEdgePointer[newNodeIndex].push_back(e);
//					}
//				}
//			}
//		}
//		for(int j = 0; j < endNodes.size(); j++)	//Construct new SuperEdges and cleanup
//		{
//			uint32_t touchedNodeIndex = endNodes.at(j);
//			newEdges[j].TargetNode = &newNodes[touchedNodeIndex];
//			newEdges[j].Capasity = edgeCapacities[touchedNodeIndex];
//			newEdges[j].Tag = new GRSuperEdgeTag();
//
//			Edge** subEdgePointers = new Edge*[oldEdgePointer[touchedNodeIndex].size()];
//			for(int k = 0; k < oldEdgePointer[touchedNodeIndex].size(); k++)
//			{
//				Edge* e = oldEdgePointer[touchedNodeIndex].at(k);
//				e->GetTag<GREdgeTag*>()->superEdge = &newEdges[j];
//				subEdgePointers[k] = e;
//			}
//			newEdges[j].GetTag<GRSuperEdgeTag*>()->subEdges = subEdgePointers;
//			newEdges[j].GetTag<GRSuperEdgeTag*>()->edgeCount = oldEdgePointer[touchedNodeIndex].size();
//
//			edgeCapacities[touchedNodeIndex] = UINT32_MAX;
//		}
//		newNodes[i].Edges = newEdges;
//		newNodes[i].EdgeCount = nrEdgesPrComponent[i];
//
//		if(nrEdgesPrComponent[i] != endNodes.size())
//			return NULL;
//
//		delete oldEdgePointer;
//	}
//
//	uint32_t edgeCount = 0;
//	for(Node* n = newNodes; n < newNodes + stronglyConnectedComponents->size(); n++) //Set up linked edges
//	{
//		edgeCount += n->EdgeCount;
//		for(Edge* e = n->Edges; e < n->Edges + n->EdgeCount; e++)
//		{
//			e->LinkedEdge = e->GetTag<GRSuperEdgeTag*>()->subEdges[0]->LinkedEdge->GetTag<GREdgeTag*>()->superEdge;
//		}
//	}
//
//	Graph* output = new Graph();
//	output->NodeCount = stronglyConnectedComponents->size();
//	output->Nodes = newNodes;
//	output->EdgeCount = edgeCount;
//	return output;
//}

//uint32_t* GoldbergRao1998::CalcNrSuperEdges(vector<vector<uint32_t>>* stronglyConnectedComponents, uint32_t* translationMatrixOldToNewNode)
//{
//	uint32_t* nrEdges = new uint32_t[stronglyConnectedComponents->size()];
//	bool* edgeSeen = new bool[stronglyConnectedComponents->size()];
//	vector<uint32_t> nodesTouched;
//
//	for(int i = 0; i < stronglyConnectedComponents->size(); i++)
//	{
//		nrEdges[i] = 0;
//		edgeSeen[i] = false;
//	}
//
//	for(int i = 0; i < stronglyConnectedComponents->size(); i++)
//	{
//		nodesTouched.clear();
//		vector<uint32_t> component = stronglyConnectedComponents->at(i);
//		for(int j = 0; j < component.size(); j++)
//		{
//			uint32_t nodeIndex = component.at(j);
//			Node* node = &m_graph->Nodes[nodeIndex];
//			uint32_t nodeDistance = node->GetTag<GRTag*>()->distance;
//			
//
//			for(Edge* e = node->Edges; e < node->Edges + node->EdgeCount; e++)
//			{
//				uint32_t newNodeIndex = translationMatrixOldToNewNode[m_graph->IndexOf(e->TargetNode)];
//				if(newNodeIndex == i) //The edge goes to an internal node in the new superNode
//				{
//					continue;
//				}
//
//				if(nodeDistance == e->TargetNode->GetTag<GRTag*>()->distance + 1 || e->Capasity > 3 * m_delta) //If an admissible edge
//				{
//					
//					if(!edgeSeen[newNodeIndex])
//					{
//						nrEdges[i]++;	
//						nrEdges[newNodeIndex]++; //Backedge in another SuperNode
//						nodesTouched.push_back(newNodeIndex);
//						edgeSeen[newNodeIndex] = true;
//					}
//				}
//			}
//		}
//
//		for(int j = 0; j < nodesTouched.size(); j++)	//Cleanup so next component can use same array.
//		{
//			edgeSeen[nodesTouched.at(j)] = false;
//		}
//	}
//
//	delete edgeSeen;
//	return nrEdges;
//}

//void GoldbergRao1998::TopologicalSort()
//{
//	uint32_t nrStronglyConnectedComponents = SCCs->size();
//	//http://en.wikipedia.org/wiki/Topological_sorting Tarjan 1976
//	uint32_t* stateVector = m_canonicalCutValues;
//	for(int i = 0; i < nrStronglyConnectedComponents; i++)
//		stateVector[i] = 0;
//
//	for(int i = 0; i < nrStronglyConnectedComponents; i++)
//	{
//		if(stateVector[i] == 0)
//		{
//			GRSuperNode* superNode = &m_SuperNodes[i];
//			TopologicalVisitRec(superNode);
//		}
//	}
//
//	Queue<Node*>* sortedQueue = m_BothTreesQueue;
//	uint32_t topologicalId = 0;
//	while(sortedQueue->GetSize() != 0)
//		sortedQueue->Dequeue()->GetTag<GRTag*>()->superNode->id = topologicalId++;
//}
//
//void GoldbergRao1998::TopologicalVisitRec(GRSuperNode* v)
//{
//	uint32_t* stateVector = m_canonicalCutValues;
//	Queue<Node*>* sortedQueue = m_BothTreesQueue;
//	if(stateVector[v->id] == 1)
//		Debug << "ERROR: Running a Topological sorting on a graph which is not a DAG\n";
//	else if(stateVector[v->id] == 0)
//	{
//		stateVector[v->id] = 1;
//		v->ResetToFirstEdge();
//		while(v->m_currentEdge != NULL)
//		{
//			Edge* e = v->m_currentEdge;
//			GRSuperNode* endpointSuperNode = e->TargetNode->GetTag<GRTag*>()->superNode;
//			if(e->Capasity > 0 && endpointSuperNode != v)
//				TopologicalVisitRec(endpointSuperNode);
//			v->GetNextEdge();
//		}
//		stateVector[v->id] = 2;
//		sortedQueue->Enqueue(v->m_subNodes[0]);
//	}
//}