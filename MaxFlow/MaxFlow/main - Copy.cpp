#include "Graph\Graph.h"
#include "MaxFlowAlgorithms\EdmundKarp\EdmundKarp.h"
#include "MaxFlowAlgorithms\Dinic\Dinic.h"
#include "Utilities\DynamicTrees\Forest.h"
#include "MaxFlowAlgorithms\KingRaoTarjan1994\KingRaoTarjan1994.h"
#include "Utilities\FlowVerifyer.h"
#include "MaxFlowAlgorithms\ModifiedKRT\ModifiedKRT.h"
#include "MaxFlowAlgorithms\KingRao1992\KingRao1992.h"
#include "MaxFlowAlgorithms\GoldbergTarjan1988\GoldbergTarjanN3.h"
#include "MaxFlowAlgorithms\GoldbergTarjan1988\GoldbergTarjanDyn.h"
#include "MaxFlowAlgorithms\EdmundKarp\EdmundKarp.h"
#include "MaxFlowAlgorithms\GoldbergRao1998\GoldbergRao1998.h"
#include "MaxFlowAlgorithms\KRTModDyn\KrtModDyn.h"
#include "MaxFlowAlgorithms\LibraryAlgorithms.h"
#include <Windows.h>

#include <cstdio>
#include <ctime>

#include <iostream>
#include <fstream>  


#include <string>    
#include <sstream>  

template <class T>
void TestSize(const char* filename)
{
	Graph* graph = Graph::LoadDigraph(filename);

	IMaxFlowAlgorithm algorithm = new T();
	size_t size = algorithm->GetByteSize(graph);

	cout << filename << "\n" << size << "\n";
	outputStream << 

	delete graph;
}

void TestSize(int algorithmNr, int graphTypeNr)
{
	std::stringstream ss;
	ss << "sizeTest alg: " << algorithmNr << ", graph: " << graphTypeNr << ".csv";
	outputStream.open(ss.str().c_str());
	outputStream << "n;Algorithm;Bytes\n";
	switch (graphTypeNr)
	{
	case 0:
		for (int size = 32; size <= 8192; size*=2)
		{
			prefix.str("");
			prefix << size;
			std::cout << "Testing connected best case graph of size " << size << std::endl;
			std::stringstream ss;
			ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cbc/cbc" << size << ".txt";
			bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
			if (!cont)
				break;
		}
		break;
	case 1:
		AutoTestConnectedWorstCaseGraphs(algorithmNr, maxDuration);
		break;
	case 2:
		AutoTestAk(algorithmNr, maxDuration);
		break;
	case 3:
		AutoTestGenRmf_aa_b(algorithmNr, maxDuration);
		break;
	case 4:
		AutoTestGenRmf_a_bb(algorithmNr, maxDuration);
		break;
	case 5:
		AutoTestGenRmf_a_b(algorithmNr, maxDuration);
		break;
	case 6:
		AutoTestWash_a_64(algorithmNr, maxDuration);
		break;
	case 7:
		AutoTestWash_b_64(algorithmNr, maxDuration);
		break;
	case 8:
		AutoTestDinicWorstCase(algorithmNr, maxDuration);
		break;
	case 9:
		AutoTestComputerVision(algorithmNr, maxDuration);
		break;
	}
}

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
	cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart);

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}

std::ofstream outputStream;
std::stringstream prefix;


template <class T>
double TestLibraryFunction(Graph* graph, const char* filename)
{
	uint32_t expectedFlow = 55104;
	{
		StartCounter();
		std::cout << "Running GT GRN: ";
		GoldbergTarjanN3RareGlobalRename2* a2 = new GoldbergTarjanN3RareGlobalRename2();		
		expectedFlow = a2->CalculateMaxFlow(graph, graph->Nodes, graph->Nodes + graph->NodeCount - 1);
		delete a2;
		double duration = GetCounter();
		
		std::cout << expectedFlow << " flow in " << duration << " seconds.\n";
		//delete graph;
	}

	double durations = 0;
	for (int i = 0; i < 3; i++)
	{
		std::clock_t start;
		double duration;
		std::cout << "Preparing...\n";

		LibraryMaxFlowAlgorithm* algorithm = new T();
		algorithm->Prepare(filename);
		std::cout << "Running...\n";

		StartCounter();

		uint32_t flow = algorithm->CalculateMaxFlow();
		delete algorithm;
		duration = GetCounter();

				

		durations += duration;

		std::cout << duration;
		outputStream << prefix.str() << ";" << duration << "\n";
		outputStream.flush();
		

	
		if (flow != expectedFlow)
		{
			std::cout << " - Invalid!!\n";
			outputStream << "Invalid";
			outputStream.flush();
			outputStream.close();
			exit(-1);
		}
		else
		{
			std::cout << " - Valid\n";
		}
		outputStream.flush();
	}
	std::cout<<"\n\n";


	return durations/3;
}


template <class T>
double Test(Graph* graph)
{
	double durations = 0;
	for (int i = 0; i < 1; i++)
	{
		Graph* original = graph->Clone();
		Graph* clone = graph->Clone();
		std::clock_t start;
		double duration;
		std::cout << "    Duration: ";

		StartCounter();

		IMaxFlowAlgorithm* algorithm = new T();
		uint32_t flow = algorithm->CalculateMaxFlow(clone, clone->Nodes, clone->Nodes + clone->NodeCount - 1);
		delete algorithm;

				
		duration = GetCounter();
		durations += duration;

		std::cout << duration;
		outputStream << prefix.str() << ";" << duration << "\n";
		outputStream.flush();
		

	
		if (!IsValidFlow(original, clone, flow))
		{
			std::cout << " - Invalid!!\n";
			outputStream << "Invalid";
			outputStream.flush();
			outputStream.close();
			exit(-1);
		}
		else
		{
			std::cout << " - Valid\n";
		}
		outputStream.flush();
		delete clone;
		delete original;
	}
	std::cout<<"\n\n";



	return durations/3;
}

Graph* GRE2SwatSpotGraph;
IMaxFlowAlgorithm* GRE2SwatSpotAlgorithm;
volatile double GRE2SwatSpotDuration;
volatile uint32_t GRE2SwatSpotFlow;

#include <thread>
DWORD WINAPI TestGRE2SweatSpotThread(LPVOID lpParam)
{
	StartCounter();
	GRE2SwatSpotFlow = GRE2SwatSpotAlgorithm->CalculateMaxFlow(GRE2SwatSpotGraph, GRE2SwatSpotGraph->Nodes, GRE2SwatSpotGraph->Nodes + GRE2SwatSpotGraph->NodeCount - 1);				
	GRE2SwatSpotDuration = GetCounter();
	return 0;
}


template <class T>
double TestGRE2SweatSpot(Graph* graph, uint32_t limit, double timeLimit)
{
	Graph* original = graph->Clone();
	Graph* clone = graph->Clone();
	std::clock_t start;
	double duration;
	uint32_t flow;
	std::cout << "    Duration: ";


	IMaxFlowAlgorithm* algorithm = new T();
	algorithm->SetCheckLimit(limit);
	GRE2SwatSpotAlgorithm = algorithm;
	GRE2SwatSpotGraph = clone;

	DWORD huh;
	HANDLE thread = 0;

	if (graph->NodeCount < 10000)
		TestGRE2SweatSpotThread(0);
	else
		thread = CreateThread(NULL, 0, TestGRE2SweatSpotThread, 0, 0, &huh);
	/*uint32_t flow = algorithm->CalculateMaxFlow(clone, clone->Nodes, clone->Nodes + clone->NodeCount - 1);
	delete algorithm;

				
	duration = GetCounter();*/

	DWORD result = WAIT_OBJECT_0;
	if (thread != 0)
		result = WaitForSingleObject(thread, timeLimit*1000);
	if (result != WAIT_OBJECT_0 && algorithm->IsCleaning)
		result = WaitForSingleObject(thread, INFINITE);

	if (result != WAIT_OBJECT_0)
	{
		duration = timeLimit;
		std::cout << duration;
		TerminateThread(thread, 0);
		std::cout << " - Expired\n";
		algorithm->CleanUp();
	}
	else
	{
		duration = GRE2SwatSpotDuration;
		flow = GRE2SwatSpotFlow;
		std::cout << duration;
		if (!IsValidFlow(original, clone, flow))
		{
			std::cout << " - Invalid!!\n";
		}
		else
		{
			std::cout << " - Valid\n";
		}
	}
	if (thread != 0)
		CloseHandle(thread);

		

	
	delete clone;
	delete original;
	return duration;
}

template <class T>
bool FindGRE2SweatSpot(Graph* graph)
{
	
	cout << "Testing " << graph->NodeCount << "\n";
	double bestTime = TestGRE2SweatSpot<T>(graph, graph->NodeCount, 60);
	if (bestTime == 60)
	{
		delete graph;
		return false;
	}
	uint32_t left = graph->NodeCount;
	uint32_t right = graph->NodeCount;
	uint32_t best = graph->NodeCount;

	
	cout << "---Exp Search---\n";


	bool goLeft = true;
	bool goRight = true;
	int iteration = 0;
	while (goLeft || goRight)
	{
		iteration++;
		if (goLeft)
		{
			cout << "Testing " << left/2 << " - ";
			double time = TestGRE2SweatSpot<T>(graph, left/2, bestTime*1.5);
			if (time < bestTime*1.1)
			{
				left /= 2;
				if (time < bestTime)
				{
					cout << "New best!\n";
					bestTime = time;
					best = left;
				}
			}
			else
				goLeft = false;
		}
		if (goRight)
		{
			cout << "Testing " << 2*right << " - ";
			double time = TestGRE2SweatSpot<T>(graph, right*2, bestTime*1.5);
			if (time < bestTime*1.1)
			{
				right *= 2;
				if (time < bestTime)
				{
					cout << "New best!\n";
					bestTime = time;
					best = right;
				}
			}
			else
				goRight = false;
		}
		if (iteration > 20)
			break;
	}
	cout << "---Bin Search---\n";
	left = best/2;
	right = best*2;
	uint32_t fraction = 2;

	for (int i = 0; i < 20; i++)
	{
		uint32_t leftTest = (left + best)/2;
		uint32_t rightTest = (right + best)/2;

		if (left == leftTest || right == rightTest)
			break;
		
		cout << "Testing " << leftTest << " - ";
		double leftTime = TestGRE2SweatSpot<T>(graph, leftTest, bestTime*1.5);
		cout << "Testing " << rightTest << " - ";
		double rightTime = TestGRE2SweatSpot<T>(graph, rightTest, bestTime*1.5);

		if (leftTime < rightTime)
			if (leftTime < bestTime)
			{
				cout << "New best!\n";
				right = best;
				bestTime = leftTime;
				best = leftTest;
			}
			else
			{
				left = leftTest;
				right = rightTest;
			}
		else
			if (rightTime < bestTime)
			{
				cout << "New best!\n";
				left = best;
				bestTime = rightTime;
				best = rightTest;
			}
			else
			{
				left = leftTest;
				right = rightTest;
			}
	}
	
	cout << "Best is " << best << "\n";
	outputStream << graph->NodeCount << ";" << best << "\n";
	outputStream.flush();
	delete graph;
	return true;
}

template <class T>
void FindGRE2SweatSpot(int iteration)
{
	{
		std::stringstream ss;
		ss << typeid(T).name() << "GenRmf a bb (" << iteration << ").csv";
		outputStream.open(ss.str().c_str());
	}
	for (int e = 4; e <= 19; e++)	
	{
		int a = round(pow(2, e/4.0));
		int b = round(pow(2, e/2.0));	
		int n = a*a*b;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
		bool cont = FindGRE2SweatSpot<T>(Graph::LoadDigraph(ss.str().c_str()));
		if (!cont)
			break;
	}
	outputStream.close();
	
	{
		std::stringstream ss;
		ss << typeid(T).name() << "GenRmf aa b (" << iteration << ").csv";
		outputStream.open(ss.str().c_str());
	}
	for (int e = 4; e <= 18; e++)	
	{	
		int a = round(pow(2, 2*e/5.0));
		int b = round(pow(2, e/5.0));
		int n = a*a*b;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
		bool cont = FindGRE2SweatSpot<T>(Graph::LoadDigraph(ss.str().c_str()));
		if (!cont)
			break;
	}
	outputStream.close();
	
	{
		std::stringstream ss;
		ss << typeid(T).name() << "GenRmf a b (" << iteration << ").csv";
		outputStream.open(ss.str().c_str());
	}
	for (int e = 4; e <= 18; e++)	
	{
		int a = round(pow(2, e/3.0));
		int b = round(pow(2, e/3.0));
		int n = a*a*b;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
		bool cont = FindGRE2SweatSpot<T>(Graph::LoadDigraph(ss.str().c_str()));
		if (!cont)
			break;
	}
	outputStream.close();
	
	{
		std::stringstream ss;
		ss << typeid(T).name() << "GenRmf Wash b64 (" << iteration << ").csv";
		outputStream.open(ss.str().c_str());
	}
	for (int size = 256; size <= 1048576; size*=2)
	{
		int a = size>>6;
		int b = 64;
		int n = a*b + 2;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
		bool cont = FindGRE2SweatSpot<T>(Graph::LoadDigraph(ss.str().c_str()));
		if (!cont)
			break;
	}
	outputStream.close();
	
	{
		std::stringstream ss;
		ss << typeid(T).name() << "GenRmf Wash a64 (" << iteration << ").csv";
		outputStream.open(ss.str().c_str());
	}
	for (int size = 256; size <= 1048576; size*=2)
	{
		int a = 64;
		int b = size>>6;
		int n = a*b + 2;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
		bool cont = FindGRE2SweatSpot<T>(Graph::LoadDigraph(ss.str().c_str()));
		if (!cont)
			break;
	}
	outputStream.close();
}

void TestAll(Graph* graph)
{
	std::string oldPrefix = prefix.str();
	/*
	std::cout << "Testing Edmund Karp\n";	
	prefix.str("");
	prefix << oldPrefix << ";EK";
	Test<EdmundKarp>(graph);
	
	std::cout << "Testing Dinic\n";
	prefix.str("");
	prefix << oldPrefix << ";Dinic";
	Test<Dinic>(graph);
		
	std::cout << "Testing GT N3\n";
	prefix.str("");
	prefix << oldPrefix << ";GT N3";
	Test<GoldbergTarjanN3>(graph);
		
	std::cout << "Testing GT Dyn\n";
	prefix.str("");
	prefix << oldPrefix << ";GT Dyn";
	Test<GoldbergTarjanDyn>(graph);
		
	std::cout << "Testing KR1992\n";
	prefix.str("");
	prefix << oldPrefix << ";KR1992";
	Test<KR1992>(graph);
	
	std::cout << "Testing Modified KR\n";
	prefix.str("");
	prefix << oldPrefix << ";KR Mod";
	Test<ModifiedKRT>(graph);
		*/
	std::cout << "Testing GR\n";
	prefix.str("");
	prefix << oldPrefix << ";GR";
	Test<GoldbergRao1998>(graph);
	

	/*
	outputStream << oldPrefix <<";KR1992;\n";
	outputStream << oldPrefix <<";KR1992;\n";
	outputStream << oldPrefix <<";KR1992;\n";
	outputStream.flush();
	*/

}

void AddCap(int** capacities, int** residualCapacities, int from, int to, int cap)
{
	int flow = min(residualCapacities[from][to], cap);
	residualCapacities[from][to] -= flow;
	residualCapacities[to][from] += flow;
	cap -= flow;
	capacities[from][to] += cap;
	residualCapacities[to][from] += cap;
}

void TestDinicWorstCase()
{
	for (int e = 2; e < 14; e++)
	{
		int size = 1 << e;
		int nodeCount = size + 2;
		cout << "NodeCount: " << nodeCount << "\n";

		int paths = 0;

		vector<int>* layers = new vector<int>[size];
	
		int** capacities = new int*[nodeCount];	
		int** residualCapacities = new int*[nodeCount];	
		for (int n1 = 0; n1 < nodeCount; n1++)
		{
			capacities[n1] = new int[nodeCount];
			residualCapacities[n1] = new int[nodeCount];
			for (int n2 = 0; n2 < nodeCount; n2++)
			{
				capacities[n1][n2] = 0;
				residualCapacities[n1][n2] = 0;
			}
		}
		paths++;
		AddCap(capacities, residualCapacities, 0, nodeCount - 1, 1);



		for (int i = 0; i < size; i++)
			layers[0].push_back(i + 1);

		int oldLayerSize = size;
		while (true) 
		{

			int maxLayerSize = 0;
			int firstMaxLayer = 0;
			int lastMaxLayer = 0;
			int lastLayer = size - 1;
			for (int i = 0; i < size; i++)
			{
				int layerSize = layers[i].size();


				if (layerSize > maxLayerSize)
				{
					firstMaxLayer = i;
					maxLayerSize = layerSize;
					lastMaxLayer = i;
				}
				else if (layerSize == maxLayerSize)
				{
					lastMaxLayer = i;
				}
				else if (layerSize == 0)
				{
					lastLayer = i - 1;
					break;
				}
			}
		


			if (maxLayerSize == 1)
			{
				AddCap(capacities, residualCapacities, 0, layers[0].at(0), 1);
				for (int i = 0; i < size - 1; i++)				
					AddCap(capacities, residualCapacities, layers[i].at(0), layers[i + 1].at(0), 1);
				AddCap(capacities, residualCapacities, layers[lastLayer].at(0), nodeCount - 1, 1);
				break;
			}

			//Top nodes
			int cap = 0;
			if (lastMaxLayer == lastLayer)
			{
				for (int row = 0; row < maxLayerSize/2; row++)
				{
					paths++;
					AddCap(capacities, residualCapacities, layers[lastMaxLayer].at(row), nodeCount - 1, 1);
				}
				cap = 1;
			}
			else
			{
				for (int sourceRow = 0; sourceRow < maxLayerSize/2; sourceRow++)
				for (int targetRow = 0; targetRow < maxLayerSize/2; targetRow++)
				{
					paths++;
					AddCap(capacities, residualCapacities, layers[lastMaxLayer].at(sourceRow), layers[lastMaxLayer + 1].at(targetRow), 1);
				}
				cap = maxLayerSize/2;
			}
		
			for (int i = lastMaxLayer - 1; i >= 0; i--)
			{
				//Send required to next top
				int reqCap = cap / (maxLayerSize/2);
				int restCap = cap % (maxLayerSize/2);
				reqCap = 0;
				restCap = cap;
				for (int sourceRow = 0; sourceRow < maxLayerSize/2; sourceRow++)
				{
					for (int targetRow = 0; targetRow < maxLayerSize/2; targetRow++)
					{
						AddCap(capacities, residualCapacities, layers[i].at(sourceRow), layers[i + 1].at(targetRow), reqCap);
					}
					AddCap(capacities, residualCapacities, layers[i].at(sourceRow), layers[i + 1].at(sourceRow), restCap);
				}
				if (i >= firstMaxLayer - 1)
				{
					//Send 1 to next bottom
					for (int sourceRow = 0; sourceRow < maxLayerSize/2; sourceRow++)
					for (int targetRow = maxLayerSize/2; targetRow < maxLayerSize; targetRow++)
					{
						paths++;
						AddCap(capacities, residualCapacities, layers[i].at(sourceRow), layers[i + 1].at(targetRow), 1);
					}
					cap += maxLayerSize/2;
				}
			}

		
			for (int row = 0; row < maxLayerSize/2; row++)
			{
				AddCap(capacities, residualCapacities, 0, layers[0].at(row), cap);
			}

			if (firstMaxLayer == 0)
			{
				for (int targetRow = maxLayerSize/2; targetRow < maxLayerSize; targetRow++)
				{
					paths++;
					AddCap(capacities, residualCapacities, 0, layers[0].at(targetRow), 1);
				}
			}


			//Bottom nodes		
			if (firstMaxLayer == 0)
				cap = 1 - maxLayerSize/2;
			else
				cap = 0;

			for (int i = firstMaxLayer; i < lastLayer; i++)
			{
				if (i <= lastMaxLayer + 1)
					cap += maxLayerSize/2;
				int reqCap = cap / (maxLayerSize/2);
				int restCap = cap % (maxLayerSize/2);
				reqCap = 0;
				restCap = cap;

				for (int s1 = 0; s1 < maxLayerSize/2; s1++)
				{
					int sourceRow = i <= lastMaxLayer ? maxLayerSize/2 + s1 : s1;
					int targetRow = i < lastMaxLayer ? maxLayerSize/2 + s1 : s1;
					AddCap(capacities, residualCapacities, layers[i].at(sourceRow), layers[i + 1].at(targetRow), restCap);
				
					for (int s2 = 0; s2 < maxLayerSize/2; s2++)
					{
						targetRow = i < lastMaxLayer ? maxLayerSize/2 + s2 : s2;
						AddCap(capacities, residualCapacities, layers[i].at(sourceRow), layers[i + 1].at(targetRow), reqCap);
					}
				}
			}
		
			if (lastLayer <= lastMaxLayer + 1)
				cap += maxLayerSize/2;

		
			for (int s1 = 0; s1 < maxLayerSize/2; s1++)
			{
				int sourceRow = lastLayer == lastMaxLayer ? maxLayerSize/2 + s1 : s1;
				AddCap(capacities, residualCapacities, layers[lastLayer].at(sourceRow), nodeCount - 1, cap);
			}




			//Move layers
			for (int i = lastLayer; i >= firstMaxLayer; i--)
			{
				for (int s1 = 0; s1 < maxLayerSize/2; s1++)
				{
					int sourceRow = i <= lastMaxLayer ? maxLayerSize/2 + s1 : s1;
					layers[i + 1].push_back(layers[i].at(sourceRow));
				}
				for (int j = maxLayerSize/2; j < maxLayerSize; j++)
					layers[i].pop_back();
			}
		}
	
		cout << "Paths: " << paths << "\n";
		continue;
	
		Graph* graph = new Graph();
		graph->NodeCount = nodeCount;
		graph->EdgeCount = 0;
		graph->Nodes = new Node[graph->NodeCount];
		for (int n1 = 0; n1 < nodeCount; n1++)
		{
			Node* node = graph->Nodes + n1;
			node->EdgeCount = 0;
			for (int n2 = 0; n2 < nodeCount; n2++)
			{
				if (capacities[n1][n2] > 0 || capacities[n2][n1] > 0)
					node->EdgeCount++;
			}
			node->Edges = new Edge[node->EdgeCount];
			graph->EdgeCount += node->EdgeCount;
			Edge* edge = node->Edges;
			for (int n2 = 0; n2 < nodeCount; n2++)
			{
				if (capacities[n1][n2] == 0 && capacities[n2][n1] == 0)
					continue;
				edge->Capasity = capacities[n1][n2];
				edge->TargetNode = graph->Nodes + n2;
				edge->Tag = 0;
				edge++;
			}
		}

		for (Node* n1 = graph->Nodes; n1 < graph->Nodes + graph->NodeCount; n1++)
			for (Edge* e1 = n1->Edges; e1 < n1->Edges + n1->EdgeCount; e1++)
				for (Edge* e2 = e1->TargetNode->Edges; e2 < e1->TargetNode->Edges + e1->TargetNode->EdgeCount; e2++)
					if (e2->TargetNode == n1)
						e1->LinkedEdge = e2;

		graph->AssertConsistent();
		
		//(new Dinic())->CalculateMaxFlow(graph, graph->Nodes, graph->Nodes + graph->NodeCount - 1);
		
		std::stringstream ss;
		ss << "D:\\Users\\Beier\\Documents\\MasterThesis\\Graphs\\badDinic\\badDinic" << nodeCount << ".txt";
		graph->SaveDigraph(ss.str().c_str());
	}
}

void TestKRT()
{
	std::cout << "Starting\n";
	srand(time(0));
	for (uint32_t size = 8; size < 10000000; size *= 2)
	for (uint32_t seed = 0; seed < 5 ; seed++)
	//for (uint32_t s = 1; s < 33; s++)
	//for (uint32_t seed = 0; ; seed++)//55 //869
	{
		srand(seed);
		/*uint32_t nodesPerLayer = s;
		uint32_t layers = s;
		uint32_t size = nodesPerLayer *layers + 2;*/
		std::cout << "Making graph\n";
		Graph* graph = Graph::GenerateGraph(GraphGenerator::GenerateGraphCustom(1), GraphGenerator::RandomCapacityGenerator(2, 20), size);
		//graph->SaveBin("TestGraph.bin");
		//std::cout << "Done making graph\n";
		//graph->SaveJpg("D:\\Users\\Beier\\Documents\\MasterThesis\\MaxFlow\\Graphs\\TestLayeredGraph.jpg");

		std::cout << "Size:" << size;
		std::stringstream ss;
		//ss << "Graph1.bin";
		//ss << "RandomGraph" << size << ".bin";
		//ss << "LayeredGraph" << nodesPerLayer << "x" << layers << ".bin";
		//graph->SaveBin(ss.str().c_str());
		//Graph* graph = Graph::LoadBin(ss.str().c_str());

		//Edges = tightness * maxEdges + (1 - tightness)*minEdges
		//Edges = tightness * (n-1)*n + (1 - tightness)*n
		//Edges = tightness * (n-1)*n + n - tightness*n
		//Edges = tightness * (n-2)*n + n

		//tightness = Edges-n/((n-2)*n)



		double tightness = (graph->EdgeCount - graph->NodeCount)/(double)(graph->NodeCount*graph->NodeCount - 2*graph->NodeCount);
		
		std::cout << ", Tightness: " << tightness << "\n";
		
		/*std::cout << "Testing Edmund Karp\n";
		Test<EdmundKarp>(graph);

		std::cout << "Testing Dinic\n";
		Test<Dinic>(graph);
		*/
		std::cout << "Testing KR1992\n";
		Test<GoldbergTarjanDynGlobalRename>(graph);
		/*
		std::cout << "Testing Modified KR\n";
		Test<ModifiedKRT>(graph);
		*/
		//graph->SaveJpg("D:\\Users\\Beier\\Documents\\MasterThesis\\MaxFlow\\Graphs\\Test3.jpg");
		
		
		std::cout<< '\n';
		delete graph;

	}
}
int round(double d)
{
	int i = (int)d;
	if (d - i >= 0.5)
		i++;
	return i;
}


void GenerateConnectedWorstCaseGraphs()
{
	//for (int size = 32; size <= 2048; size*=2)
	srand(time(0));
	for (int size = 2048; size <= 8192; size*=2)
	{
		std::cout << "Loading connected worst case graph of size " << size << std::endl;
		Graph* graph = Graph::GenerateGraph(GraphGenerator::GenerateConnectedGraph(), GraphGenerator::ConnectedCapacityGenerator(1, 10000, true), size);
		std::cout << "Checking consistency" << std::endl;
		graph->AssertConsistent();
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cwc/cwc" << size << ".txt";
		std::cout << "Saving" << std::endl;
		graph->SaveDigraph(ss.str().c_str());
		delete graph;
	}
}

void GenerateConnectedBestCaseGraphs()
{
	//for (int size = 32; size <= 2048; size*=2)
	srand(time(0));
	for (int size = 32; size <= 8192; size*=2)
	{
		std::cout << "Loading connected best case graph of size " << size << std::endl;
		Graph* graph = Graph::GenerateGraph(GraphGenerator::GenerateConnectedGraph(), GraphGenerator::ConnectedCapacityGenerator(1, 10000, false), size);
		std::cout << "Checking consistency" << std::endl;
		graph->AssertConsistent();
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cbc/cbc" << size << ".txt";
		std::cout << "Saving" << std::endl;
		graph->SaveDigraph(ss.str().c_str());
		delete graph;
	}
}

void GenerateAcGraphs()
{
	//for (int size = 32; size <= 2048; size*=2)
	for (int size = 2048*2; size <= 204800000; size*=2)
	{
		std::cout << "Loading ac graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac.exe -r " << size << " " << time(0) << " > D:/Users/Beier/Documents/MasterThesis/Graphs/ac/ac" << size << ".txt";
		std::system(ss.str().c_str());
		ss.str("");
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/ac" << size << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());
		graph->AssertConsistent();
		delete graph;
	}
}

void GenerateAkGraphs()
{
	for (int size = 16; size <= 16; size*=2)
	{
		std::cout << "Loading ak graph of size " << size + 2 << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak.exe " << (size - 4)/4 << " > D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size + 2 << ".txt";
		std::system(ss.str().c_str());
		ss.str("");
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size + 2 << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());
		graph->AssertConsistent();
	}
}

void GenerateGenRmf(int a, int b)
{
	int n = a*a*b;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf.exe";
	ss << " -a " << a;
	ss << " -b " << b;
	ss << " -c1 " << 1;
	ss << " -c2 " << 10000;
	ss << " > D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
	std::system(ss.str().c_str());
}

void GenerateGenRmf()
{
	for (int e = 4; e <= 20; e++)	
	//for (int size = 16; size <= 1048576; size*=2)	
	{
		int n = 1<<e;
		
		std::cout << "Generating genrmf graph of size " << n << std::endl;
		
		//b=a^2
		GenerateGenRmf(round(pow(2, e/4.0)), round(pow(2, e/2.0)));
		//a=b^2
		GenerateGenRmf(round(pow(2, 2*e/5.0)), round(pow(2, e/5.0)));
		//a=b
		GenerateGenRmf(round(pow(2, e/3.0)), round(pow(2, e/3.0)));
	}
}

void GenerateTgGraphs()
{
	for (int size = 32; size <= 1048576; size*=2)
	{
		std::cout << "Loading tg graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/tg.exe " << size << " 10000 > D:/Users/Beier/Documents/MasterThesis/Graphs/tg/tg" << size << ".txt";
		std::system(ss.str().c_str());
		ss.str("");
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/tg/tg" << size << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());
		graph->AssertConsistent();
	}
}


void GenerateWash(int a, int b)
{
	int n = a*b + 2;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash.exe";
	ss << " " << time(NULL);
	ss << " 2";
	ss << " " << a;
	ss << " " << b;
	ss << " 10000";
	ss << " > D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
	std::system(ss.str().c_str());
	ss.str("");
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
	Graph* graph = Graph::LoadDigraph(ss.str().c_str());
	graph->AssertConsistent();
	delete graph;
}

void GenerateWash()
{
	for (int size = 256; size <= 1048576; size*=2)
	{		
		std::cout << "Generating wash graph of size " << size << std::endl;
		
		GenerateWash(64, size>>6); 
		GenerateWash(size>>6, 64);
	}

}


void TestAk()
{
	outputStream.open("outAKGR.csv");
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 18; size <= 1048578; size= size*2 - 2)
	//for (int size = 32770; size <= 1048578; size= size*2 - 2)
	{
		std::cout << "Testing AK graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());

		TestAll(graph);

		delete graph;
	}
}

void TestAc()
{
	outputStream.open("outAcGR.csv");
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 32; size <= 8192; size*=2)
	{
		std::cout << "Testing AC graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/ac" << size << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());
		
		TestAll(graph);

		delete graph;
	}
}

void TestGenRmf(int a, int b)
{
	prefix.str("");
	prefix << a << ";" << b;
	int n = a*a*b;
	std::cout << "Testing genrmf graph of size " << a << "x" << a << "x" << b << "=" << n << std::endl;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf/genrmf_a" << a << "b" << b << "n" << n << ".txt";
	Graph* graph = Graph::LoadDigraph(ss.str().c_str());
	TestAll(graph);
	delete graph;
}


void TestGenRmf()
{
	outputStream.open("outGenRmf2GR.csv");
	outputStream << "a;b;Algorithm;Time\n";
	for (int e = 4; e <= 20; e++)	
	{		
		//b=a^2
		//TestGenRmf(round(pow(2, e/4.0)), round(pow(2, e/2.0)));
		//a=b^2
		TestGenRmf(round(pow(2, 2*e/5.0)), round(pow(2, e/5.0)));
		//a=b
		//TestGenRmf(round(pow(2, e/3.0)), round(pow(2, e/3.0)));
	}
}


void TestWash(int a, int b)
{
	int n = a*b + 2;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
	Graph* graph = Graph::LoadDigraph(ss.str().c_str());
	TestAll(graph);
	delete graph;
}


void TestWash()
{
	for (int size = 256; size <= 1048576; size*=2)
	{
		std::cout << "Testing wash graphs of size " << size << std::endl;
		
		TestWash(64, size>>6); 
		TestWash(size>>6, 64);
	}
}


bool AutoTest(const char* filename, int algorithmNr, int maxDuration)
{
	std::string oldPrefix = prefix.str();
	prefix.str("");
	Graph* graph = Graph::LoadDigraph(filename);
	double duration = 0;
	
	switch (algorithmNr)
	{
	case 0:
		cout << "Testing EK\n";
		prefix << oldPrefix << ";EK";
		duration = Test<EdmundKarp>(graph);
		break;
	case 1:
		cout << "Testing Dinic\n";
		prefix << oldPrefix << ";Dinic";
		duration = Test<Dinic>(graph);
		break;
	case 2:
		cout << "Testing GT N3\n";
		prefix << oldPrefix << ";GT N3";
		duration = Test<GoldbergTarjanN3>(graph);
		break;
	case 3:
		cout << "Testing GT Dyn\n";
		prefix << oldPrefix << ";GT Dyn";
		duration = Test<GoldbergTarjanDyn>(graph);
		break;
	case 4:
		cout << "Testing KR\n";
		prefix << oldPrefix << ";KR";
		duration = Test<KR1992>(graph);
		break;
	case 5:
		cout << "Testing KR Mod\n";
		prefix << oldPrefix << ";KR Mod";
		duration = Test<ModifiedKRT>(graph);
		break;
	case 6:
		cout << "Testing KR Mod Dyn\n";
		prefix << oldPrefix << ";KR Mod Dyn";
		duration = Test<KrtModDyn>(graph);
		break;
	case 7:
		cout << "Testing GR\n";
		prefix << oldPrefix << ";GR";
		duration = Test<GoldbergRao1998>(graph);
		break;
	case 8:
		cout << "Testing GT N3 Global Rename\n";
		prefix << oldPrefix << ";GTN3GR";
		duration = Test<GoldbergTarjanN3GlobalRename>(graph);
		break;
	case 9:
		cout << "Testing GT Dyn Global Rename\n";
		prefix << oldPrefix << ";GTDGR";
		duration = Test<GoldbergTarjanDynGlobalRename>(graph);
		break;
	case 10:
		cout << "Testing KR1992 Global Rename\n";
		prefix << oldPrefix << ";KR1992GR";
		duration = Test<KR1992GlobalRename>(graph);
		break;
	case 11:
		cout << "Testing KR1992 Mod Global Rename\n";
		prefix << oldPrefix << ";KR1992M3GR";
		duration = Test<ModifiedKRTGlobalRename>(graph);
		break;
	case 12:
		cout << "Testing KR1992 Mod Dyn Global Rename\n";
		prefix << oldPrefix << ";KR1992MDGR";
		duration = Test<KrtModDynGlobalRename>(graph);
		break;
	case 13:
		cout << "Testing GT N3 Rare Global Rename\n";
		prefix << oldPrefix << ";GTN3RGR";
		duration = Test<GoldbergTarjanN3RareGlobalRename>(graph);
		break;
	case 14:
		cout << "Testing GT Dyn Rare Global Rename\n";
		prefix << oldPrefix << ";GTDRGR";
		duration = Test<GoldbergTarjanDynRareGlobalRename>(graph);
		break;
	case 15:
		cout << "Testing KR1992 Rare Global Rename\n";
		prefix << oldPrefix << ";KR1992RGR";
		duration = Test<KR1992RareGlobalRename>(graph);
		break;
	case 16:
		cout << "Testing KR1992 Rare Mod Global Rename\n";
		prefix << oldPrefix << ";KR1992M3RGR";
		duration = Test<ModifiedKRTRareGlobalRename>(graph);
		break;
	case 17:
		cout << "Testing KR1992 Rare Mod Dyn Global Rename\n";
		prefix << oldPrefix << ";KR1992MDRGR";
		duration = Test<KrtModDynRareGlobalRename>(graph);
		break;
	case 18:
		cout << "Testing GT GRE2\n";
		prefix << oldPrefix << ";GTGRE2";
		duration = Test<GoldbergTarjanN3RareGlobalRename2>(graph);
		break;
	case 19:
		cout << "Testing GT D GRE2\n";
		prefix << oldPrefix << ";GTDGRE2";
		duration = Test<GoldbergTarjanDynRareGlobalRename2>(graph);
		break;
	case 20:
		cout << "Testing KR LM GRE2\n";
		prefix << oldPrefix << ";KRLMGRE2";
		duration = Test<ModifiedKRTRareGlobalRename2>(graph);
		break;
	case 21:
		cout << "Testing KR LM D GRE2\n";
		prefix << oldPrefix << ";KRLMGRE2";
		duration = Test<KrtModDynRareGlobalRename2>(graph);
		break;
	case 22:
		cout << "Testing EK Lib\n";
		prefix << oldPrefix << ";EKLib";		
		duration = TestLibraryFunction<LibraryEdmondsKarpAlgorithm>(graph, filename);
		break;
	case 23:
		cout << "Testing GT Lib\n";
		prefix << oldPrefix << ";GTLib";		
		duration = TestLibraryFunction<LibraryPushRelabelAlgorithm>(graph, filename);
		break;
	case 24:
		cout << "Testing BK Lib\n";
		prefix << oldPrefix << ";BKLib";		
		duration = TestLibraryFunction<LibraryBoykovKolmogorovAlgorithm>(graph, filename);
		break;
	}

	delete graph;
	return duration < maxDuration;
}



void AutoTestDinicWorstCase(int algorithmNr, int maxDuration)
{
	for (int e = 2; e < 14; e++)
	{
		int size = 1 << e;
		int nodeCount = size + 2;
		std::stringstream ss;
		ss << "D:\\Users\\Beier\\Documents\\MasterThesis\\Graphs\\badDinic\\badDinic" << nodeCount << ".txt";
		prefix.str("");
		prefix << nodeCount;
		std::cout << "Testing bad dinic size " << size << std::endl;
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestConnectedWorstCaseGraphs(int algorithmNr, int maxDuration)
{
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 32; size <= 8192; size*=2)
	{
		prefix.str("");
		prefix << size;
		std::cout << "Testing connected worst case graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cwc/cwc" << size << ".txt";
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestConnectedBestCaseGraphs(int algorithmNr, int maxDuration)
{
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 32; size <= 8192; size*=2)
	{
		prefix.str("");
		prefix << size;
		std::cout << "Testing connected best case graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cbc/cbc" << size << ".txt";
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestAcEasy(int algorithmNr, int maxDuration)
{
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 32; size <= 8192; size*=2)
	{
		std::cout << "Testing easy AC graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acEasy" << size << ".txt";
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestAcHard(int algorithmNr, int maxDuration)
{
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 32; size <= 8192; size*=2)
	{
		std::cout << "Testing hard AC graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acHard" << size << ".txt";
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestAk(int algorithmNr, int maxDuration)
{
	outputStream << "Size;Algorithm;Time\n";
	for (int size = 18; size <= 1048578; size= size*2 - 2)
	//for (int size = 8194; size <= 8194; size= size*2 - 2)
	//for (int size = 4098; size <= 4098; size= size*2 - 2)
	{
		std::cout << "Testing AK graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size << ".txt";
		bool cont = AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

bool AutoTestGenRmf(int a, int b, int algorithmNr, int maxDuration)
{
	prefix.str("");
	prefix << a << ";" << b;
	int n = a*a*b;
	std::cout << "Testing genrmf graph of size " << a << "x" << a << "x" << b << "=" << n << std::endl;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
	return AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
}

void AutoTestGenRmf_aa_b(int algorithmNr, int maxDuration)
{
	outputStream << "a;b;Algorithm;Time\n";
	for (int e = 4; e <= 20; e++)	
	{		
		bool cont = AutoTestGenRmf(round(pow(2, e/4.0)), round(pow(2, e/2.0)), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestGenRmf_a_bb(int algorithmNr, int maxDuration)
{
	outputStream << "a;b;Algorithm;Time\n";
	for (int e = 4; e <= 20; e++)	
	{		
		bool cont = AutoTestGenRmf(round(pow(2, 2*e/5.0)), round(pow(2, e/5.0)), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void AutoTestGenRmf_a_b(int algorithmNr, int maxDuration)
{
	outputStream << "a;b;Algorithm;Time\n";
	for (int e = 4; e <= 20; e++)	
	{		
		bool cont = AutoTestGenRmf(round(pow(2, e/3.0)), round(pow(2, e/3.0)), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}


bool AutoTestWash(int a, int b, int algorithmNr, int maxDuration)
{
	int n = a*b + 2;
	std::stringstream ss;
	ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a" << a << "b" << b << "n" << n << ".txt";
	return AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
}


void AutoTestWash_a_64(int algorithmNr, int maxDuration)
{
	for (int size = 256; size <= 1048576; size*=2)
	{
		std::cout << "Testing wash graphs of size " << size << std::endl;
		prefix.str("");
		prefix << size;
		bool cont = AutoTestWash(64, size>>6, algorithmNr, maxDuration); 
		if (!cont)
			break;
	}
}


void AutoTestWash_b_64(int algorithmNr, int maxDuration)
{
	for (int size = 256; size <= 1048576; size*=2)
	{
		std::cout << "Testing wash graphs of size " << size << std::endl;
		
		prefix.str("");
		prefix << size;
		bool cont = AutoTestWash(size>>6, 64, algorithmNr, maxDuration); 
		if (!cont)
			break;
	}
}

#include "../Libraries/boost/filesystem.hpp"
void ListDirRec(const char *Dir, vector<string>& DirFileList, const char* ext)
{    
	boost::filesystem::recursive_directory_iterator rdi(Dir);  
	boost::filesystem::recursive_directory_iterator end_rdi;

	DirFileList.empty();

	string ext_str0(ext);   
	for (; rdi != end_rdi; rdi++)
	{
		cout << (*rdi).path().string() << endl;

		if (ext_str0.compare((*rdi).path().extension().string()) == 0)
		{
			DirFileList.push_back((*rdi).path().string());
		}
	}
}

void AutoTestComputerVision(int algorithmNr, int maxDuration)
{
	vector<string> files;
	ListDirRec("D:\\Users\\Beier\\Documents\\MasterThesis\\Graphs\\ComputerVision", files, "max");
	
	outputStream << "File;Algorithm;Time\n";
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		std::cout << "Testing CV graph " << *it << std::endl;
		prefix.str("");
		prefix << *it;
		bool cont = AutoTest(it->c_str(), algorithmNr, maxDuration);
		if (!cont)
			break;
	}
}

void ProcessArgs(int argc, char* argv[])
{
	SetErrorMode(GetErrorMode () | SEM_NOGPFAULTERRORBOX);
	_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	for (int i = 0; i < argc; i++)
		cout << argv[i] << "\n";
	if (argc != 5)
		return;
	int graphTypeNr = atoi(argv[1]);
	int algorithmNr = atoi(argv[2]);
	int maxDuration = atoi(argv[3]);
	char* outputFile = argv[4];

	
	outputStream.open(outputFile);

	switch (graphTypeNr)
	{
	case 0:
		AutoTestConnectedBestCaseGraphs(algorithmNr, maxDuration);
		break;
	case 1:
		AutoTestConnectedWorstCaseGraphs(algorithmNr, maxDuration);
		break;
	case 2:
		AutoTestAk(algorithmNr, maxDuration);
		break;
	case 3:
		AutoTestGenRmf_aa_b(algorithmNr, maxDuration);
		break;
	case 4:
		AutoTestGenRmf_a_bb(algorithmNr, maxDuration);
		break;
	case 5:
		AutoTestGenRmf_a_b(algorithmNr, maxDuration);
		break;
	case 6:
		AutoTestWash_a_64(algorithmNr, maxDuration);
		break;
	case 7:
		AutoTestWash_b_64(algorithmNr, maxDuration);
		break;
	case 8:
		AutoTestDinicWorstCase(algorithmNr, maxDuration);
		break;
	case 9:
		AutoTestComputerVision(algorithmNr, maxDuration);
		break;
	}

	outputStream.close();
}

uint32_t GetOutCap(Node* node)
{
	uint32_t ret = 0;
	for (int i = 0; i < node->EdgeCount; i++)
	{
		ret += node->Edges[i].Capasity;
	}
	return ret;
}

uint32_t GetInCap(Node* node)
{
	uint32_t ret = 0;
	for (int i = 0; i < node->EdgeCount; i++)
	{
		ret += node->Edges[i].LinkedEdge->Capasity;
	}
	return ret;
}

void SwapIOCapacities(Graph* graph)
{
	for (int i = 0; i < graph->Nodes[0].EdgeCount; i++)
	{
		uint32_t* p1 = &graph->Nodes[0].Edges[i].Capasity;
		uint32_t* p2 = &graph->Nodes[graph->NodeCount - 1].Edges[i].LinkedEdge->Capasity;

		uint32_t temp = *p1;
		*p1 = *p2;
		*p2 = temp;
	}
}



int main(int argc, char* argv[])
{
	cout << "sizeof(Node)" << sizeof(Node) << "\n";
	cout << "sizeof(Edge)" << sizeof(Edge) << "\n";
	cout << "sizeof(Graph)" << sizeof(Graph) << "\n";
	cout << "sizeof(BFSTag)" << sizeof(BFSTag) << "\n";

	
	argv = new char*[5];
	argv[0] = "";
	argv[1] = "9";//Graph
	argv[2] = "18";//Algorithm
	argv[3] = "1200";
	argv[4] = "2013-11-11\\asdf.csv";
	argc = 5;
	
	ProcessArgs(argc, argv);
	return 0;

	outputStream.open("outTest.csv");
	//for (int e = 4; e <= 16; e++)	
	{		
		/*uint32_t a = round(pow(2, e/4.0));
		uint32_t b = round(pow(2, e/2.0));
		prefix.str("");
		prefix << a << ";" << b;
		int n = a*a*b;
		std::cout << "Testing genrmf graph of size " << a << "x" << a << "x" << b << "=" << n << std::endl;*/
		std::stringstream ss;
		//ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a" << a << "b" << b << "n" << n << ".txt";
		ss << "D:/Users/Beier/Downloads/liver.n6c10/liver.n6c10.max";
		const char * cStr = ss.str().c_str();
		std::cout << "Loading graph\n";
		Graph* graph = Graph::LoadDigraph( ss.str().c_str());
		LibraryCommercial3D boo;
		boo.Prepare(graph);
		
		//TestLibraryFunction<LibraryEdmondsKarpAlgorithm>(graph,  ss.str().c_str());
		TestLibraryFunction<LibraryPushRelabelAlgorithm>(graph,  ss.str().c_str());
		//delete graph;
	}

	return 0;
	 
	int algorithmNr = 1;
	int maxDuration = 1000000;
	/*
	for (int size = 32; size <= 4096; size*=2)
	{
		prefix.str("");
		prefix << size;
		std::cout << "Testing connected worst case graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cwc/cwc" << size << ".txt";
		AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
	}
	for (int size = 32; size <= 4096; size*=2)
	{
		prefix.str("");
		prefix << size;
		std::cout << "Testing connected best case graph of size " << size << std::endl;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/cbc/cbc" << size << ".txt";
		AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
	}
	for (int e = 2; e < 11; e++)
	{
		int size = 1 << e;
		int nodeCount = size + 2;
		std::stringstream ss;
		ss << "D:\\Users\\Beier\\Documents\\MasterThesis\\Graphs\\badDinic\\badDinic" << nodeCount << ".txt";
		prefix.str("");
		prefix << nodeCount;
		std::cout << "Testing bad dinic size " << size << std::endl;
		AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
	}*/
	for (int size = 18; size <= 65538; size= size*2 - 2)
	{
		std::cout << "Testing AK graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size << ".txt";
		AutoTest(ss.str().c_str(), algorithmNr, maxDuration);
	}
	for (int e = 4; e <= 16; e++)	//e=16 => n=65536
	{		
		AutoTestGenRmf(round(pow(2, e/4.0)), round(pow(2, e/2.0)), algorithmNr, maxDuration);
	}
	for (int e = 4; e <= 17; e++)	
	{		
		AutoTestGenRmf(round(pow(2, 2*e/5.0)), round(pow(2, e/5.0)), algorithmNr, maxDuration);
	}
	for (int e = 4; e <= 16; e++)	
	{		
		AutoTestGenRmf(round(pow(2, e/3.0)), round(pow(2, e/3.0)), algorithmNr, maxDuration);
	}
	for (int size = 256; size <= 65538; size*=2)
	{
		std::cout << "Testing wash graphs of size " << size << std::endl;
		prefix.str("");
		prefix << size;
		AutoTestWash(64, size>>6, algorithmNr, maxDuration); 
	}
	for (int size = 256; size <= 32770; size*=2)
	{
		std::cout << "Testing wash graphs of size " << size << std::endl;
		
		prefix.str("");
		prefix << size;
		AutoTestWash(size>>6, 64, algorithmNr, maxDuration); 
	}


	/*
	for (size_t e = 30; ; e++)
	{
		size_t size = ((size_t)1)<<e;
		std::cout <<"2^" << e << "=" << size << "\n";
		system("pause");
		delete new byte[size];
	}*/

	//TestKRT();
	//return;

	
	//GenerateGenRmf();
	/*argv = new char*[5];
	argv[0] = "";
	argv[1] = "3";//Graph
	argv[2] = "18";//Algorithm
	argv[3] = "1200";
	argv[4] = "2013-11-11\\asdf.csv";
	argc = 5;*/
	ProcessArgs(argc, argv);
	//TestDinicWorstCase();
	/*
	for (int i = 0; i < 3; i++)
	{
		FindGRE2SweatSpot<GoldbergTarjanN3RareGlobalRename2>(i);
		FindGRE2SweatSpot<GoldbergTarjanDynRareGlobalRename2>(i);
		FindGRE2SweatSpot<ModifiedKRTRareGlobalRename2>(i);
		FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(i);
	}*/

	/*for (int size = 18; size <= 1048578; size= size*2 - 2)
	{
		std::cout << "Testing AK graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak" << size << ".txt";
		FindGRE2SweatSpot<ModifiedKRTRareGlobalRename2>(Graph::LoadDigraph(ss.str().c_str()));

	}*/

	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/cwc/cwc2048.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/cbc/cbc2048.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:\\Users\\Beier\\Documents\\MasterThesis\\Graphs\\badDinic\\badDinic258.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/ak/ak16386.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a10b91n9100.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a37b6n8214.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/genrmf2/genrmf_a20b20n8000.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a64b256n16386.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a256b64n16386.txt"));
	//FindGRE2SweatSpot<KrtModDynRareGlobalRename2>(Graph::LoadDigraph("D:/Users/Beier/Documents/MasterThesis/Graphs/wash/wash_a64b64n4098.txt"));


	/*static readonly string[] algorithms = new[]
                                 {
                                     "EK",
                                     "Dinic",
                                     "GT N3",
                                     "GT Dyn",
                                     "KR",
                                     "KR Mod",
                                     "KR Mod Dyn",
                                     "GR", 
                                     "GT N3 GR",
                                     "GT D GR",
                                     "KR1992 GR",
                                     "KR1992 M GR",
                                     "KR1992 M D GR",
                                     "GT N3 RGR",
                                     "GT D RGR",
                                     "KR1992 RGR",
                                     "KR1992 M RGR",
                                     "KR1992 M D RGR",
                                 };
        static readonly string[] graphTypes = new[]
                                 {
                                     "CBC",
                                     "CWC",
                                     "AK",
                                     "GenRmf b=aa",
                                     "GenRmf a=bb",
                                     "GenRmf a=b",
                                     "Wash a=64",
                                     "Wash b=64",
                                     "Bad Dinic"
                                 };*/

	//TestGenRmf(36, 6);
/*
	for (int size = 32; size <= 8192; size*=2)
	{
		std::cout << "Testing AC graph of size n=" << size << std::endl;
		prefix.str("");
		prefix << size;
		std::stringstream ss;
		ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/ac" << size << ".txt";
		Graph* graph = Graph::LoadDigraph(ss.str().c_str());
		if (GetOutCap(graph->Nodes) > GetInCap(graph->Nodes + graph->NodeCount - 1))
		{
			ss.str("");
			ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acHard" << size << ".txt";
			graph->SaveDigraph(ss.str().c_str());
			SwapIOCapacities(graph);
			ss.str("");
			ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acEasy" << size << ".txt";
			graph->SaveDigraph(ss.str().c_str());
		}
		else
		{
			ss.str("");
			ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acEasy" << size << ".txt";
			graph->SaveDigraph(ss.str().c_str());
			SwapIOCapacities(graph);
			ss.str("");
			ss << "D:/Users/Beier/Documents/MasterThesis/Graphs/ac/acHard" << size << ".txt";
			graph->SaveDigraph(ss.str().c_str());
		}
		delete graph;
	}
	*/



	/*
	std::cout << "sizeof(Node): " << sizeof(Node) << "\n";
	std::cout << "sizeof(Edge): " << sizeof(Edge) << "\n";
	TestGenRmf();*/
	//TestKRT();
	//system("pause");
	return 0;
}
