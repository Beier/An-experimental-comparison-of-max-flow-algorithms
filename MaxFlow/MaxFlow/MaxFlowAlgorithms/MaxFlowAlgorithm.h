#pragma once

#include <stdint.h>
#include "../Graph/Graph.h"

class IMaxFlowAlgorithm
{
public:
	IMaxFlowAlgorithm()
	{
		IsCleaning = false;
	}
	virtual ~IMaxFlowAlgorithm() {}
	virtual uint32_t CalculateMaxFlow(Graph* graph, Node* source, Node* target) = 0;
	virtual void SetCheckLimit(uint32_t limit) {}
	virtual void CleanUp(){}
	virtual size_t GetByteSize(Graph* g){return 0;}
	bool IsCleaning;
};
