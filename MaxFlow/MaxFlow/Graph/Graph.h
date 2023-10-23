#pragma once
#include <stdint.h>
#include <vector>
#include <functional>


struct Node;
struct Edge;
struct Graph;

typedef std::function<void(uint32_t n, std::vector<uint32_t>* edges)> GenerateGraphFunc;
typedef std::function<void(Graph* g)> GenerateCapacitiesFunc;
typedef std::function<uint32_t(uint32_t)> UpdateFunc;

class EdgeEnumerator
{
private:
	Graph* graph;
	Node* currentNode;
	Edge* currentEdge;
	int nextCount;
public:
	EdgeEnumerator(Graph* graph);
	Edge* GetCurrent();
	bool HasNext();
	bool Next();
	void Reset();
private:
	bool HasNextEdge();
	bool HasNextNode();
};

struct Edge
{
	Edge* LinkedEdge;
	Node* TargetNode;
	uint32_t Capasity;
	Edge* CloneShallow();
	void* Tag;

	template <class T>
	T GetTag()
	{
		return (T) Tag;
	}
};

struct Node
{
	uint32_t EdgeCount;
	Edge* Edges;
	void* Tag;

	uint32_t IndexOfEdge(Edge* n);
	Node* CloneShallow();
	template <class T>
	T GetTag()
	{
		return (T) Tag;
	}
};

struct Graph
{
	Node* Nodes;
	uint32_t NodeCount;
	uint32_t EdgeCount;
	
	static Graph* Load(const char* filename);
	static Graph* LoadBin(const char* filename);
	static Graph* LoadDigraph(const char* filename);
	void SaveCsv(const char* filename);
	void SaveJpg(const char* filename);
	void SaveBin(const char* filename);
	void SaveDigraph(const char* filename);
	static bool SwapEdges(Edge* e1, Edge* e2);
	static bool SwapEdgesAndPointers(Edge** e1, Edge** e2);
	uint32_t IndexOf(Node* n);
	void SortEdges();
	uint32_t GetByteSize()
	{
		return sizeof(Graph) + NodeCount*sizeof(Node) + EdgeCount*sizeof(Edge);
	}

	static Graph* GenerateGraph(GenerateGraphFunc generateGraph, GenerateCapacitiesFunc capacityGenerator, uint32_t nodeSize);
	bool IsConsistent();
	void AssertConsistent();
	Graph* Clone();
	uint32_t GetMaxCapacity();

	bool Equals(Graph* other);

	~Graph()
	{
		for (Node* node = Nodes; node < Nodes + NodeCount; node++)
			delete[] node->Edges;
		delete[] Nodes;
	}
};

namespace GraphGenerator
{
	extern UpdateFunc MultiplicationUpdate(uint32_t m);
	extern UpdateFunc AdditionUpdate(uint32_t m);
	extern GenerateCapacitiesFunc RandomCapacityGenerator(uint32_t min, uint32_t max);
	extern GenerateGraphFunc GenerateGraphCustom(double tightness);
	extern GenerateGraphFunc GenerateLayeredGraph(int layers, double tightness);
	extern GenerateGraphFunc GenerateConnectedGraph();
	extern GenerateCapacitiesFunc ConnectedCapacityGenerator(uint32_t min, uint32_t max, bool worstCase);

}

namespace Utils
{
	extern char* LoadFile(const char* filename);
}