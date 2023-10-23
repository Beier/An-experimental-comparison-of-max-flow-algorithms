#include "../Graph/Graph.h"





Graph* GenerateFlowGraph(Graph* originalGraph, Graph* residualGraph);
bool IsValidFlow(Graph* originalGraph, Graph* residualGraph, uint32_t maxFlow);