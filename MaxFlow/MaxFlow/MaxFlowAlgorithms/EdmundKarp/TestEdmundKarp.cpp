#include "EdmundKarp.h"
#include "TestEdmundKarp.h"
#include <iostream>
#include "../../Utilities/Debug.h"

namespace Tests
{/*
bool TestEdmundKarp()
	{
		Graph g;
		g.Edges = new Edge[18];
		g.EdgeCount=18;

		g.Nodes = new Node[6];
		g.Nodes[0].EdgeCount = 2;
		g.Nodes[0].Edges = new Edge[2];
		g.Nodes[1].EdgeCount = 3;
		g.Nodes[1].Edges = new Edge[3];
		g.Nodes[2].EdgeCount = 4;
		g.Nodes[2].Edges = new Edge[4];
		g.Nodes[3].EdgeCount = 2;
		g.Nodes[3].Edges = new Edge[2];
		g.Nodes[4].EdgeCount = 3;
		g.Nodes[4].Edges = new Edge[3];
		g.Nodes[5].EdgeCount = 4;
		g.Nodes[5].Edges = new Edge[4];
		g.NodeCount=6;

		//Node 0
		g.Nodes[0].Edges[0].Capasity = 16;
		g.Nodes[0].Edges[0].LinkedEdge = &g.Edges[4];
		g.Edges[0].TargetNode = &g.Nodes[1];

		g.Edges[1].Capasity = 13;
		g.Edges[1].LinkedEdge = &g.Edges[17];
		g.Edges[1].TargetNode = &g.Nodes[5];

		//Node 1
		g.Edges[2].Capasity = 12;
		g.Edges[2].LinkedEdge = &g.Edges[8];
		g.Edges[2].TargetNode = &g.Nodes[2];

		g.Edges[3].Capasity = 10;
		g.Edges[3].LinkedEdge = &g.Edges[14];
		g.Edges[3].TargetNode = &g.Nodes[5];

		g.Edges[4].Capasity = 0;
		g.Edges[4].LinkedEdge = &g.Edges[0];
		g.Edges[4].TargetNode = &g.Nodes[0];
		
		//Node 2
		g.Edges[5].Capasity = 20;
		g.Edges[5].LinkedEdge = &g.Edges[9];
		g.Edges[5].TargetNode = &g.Nodes[3];

		g.Edges[6].Capasity = 0;
		g.Edges[6].LinkedEdge = &g.Edges[11];
		g.Edges[6].TargetNode = &g.Nodes[4];

		g.Edges[7].Capasity = 9;
		g.Edges[7].LinkedEdge = &g.Edges[15];
		g.Edges[7].TargetNode = &g.Nodes[5];

		g.Edges[8].Capasity = 0;
		g.Edges[8].LinkedEdge = &g.Edges[2];
		g.Edges[8].TargetNode = &g.Nodes[1];
		
		//Node 3
		g.Edges[9].Capasity = 0;
		g.Edges[9].LinkedEdge = &g.Edges[5];
		g.Edges[9].TargetNode = &g.Nodes[2];

		g.Edges[10].Capasity = 0;
		g.Edges[10].LinkedEdge = &g.Edges[12];
		g.Edges[10].TargetNode = &g.Nodes[4];		

		//Node 4
		g.Edges[11].Capasity = 7;
		g.Edges[11].LinkedEdge = &g.Edges[6];
		g.Edges[11].TargetNode = &g.Nodes[2];

		g.Edges[12].Capasity = 4;
		g.Edges[12].LinkedEdge = &g.Edges[10];
		g.Edges[12].TargetNode = &g.Nodes[3];

		g.Edges[13].Capasity = 0;
		g.Edges[13].LinkedEdge = &g.Edges[16];
		g.Edges[13].TargetNode = &g.Nodes[5];

		//Node 5
		g.Edges[14].Capasity = 4;
		g.Edges[14].LinkedEdge = &g.Edges[3];
		g.Edges[14].TargetNode = &g.Nodes[1];

		g.Edges[15].Capasity = 0;
		g.Edges[15].LinkedEdge = &g.Edges[7];
		g.Edges[15].TargetNode = &g.Nodes[2];

		g.Edges[16].Capasity = 14;
		g.Edges[16].LinkedEdge = &g.Edges[13];
		g.Edges[16].TargetNode = &g.Nodes[4];

		g.Edges[17].Capasity = 0;
		g.Edges[17].LinkedEdge = &g.Edges[1];
		g.Edges[17].TargetNode = &g.Nodes[0];

		//for(int i = 0; i<18; i++)
		//	//cout << "capacity on edge " << i << ": " << g.Edges[i].Capasity << endl;
		//	cout << "Linked edge " << i << ": " << g.Edges[i].LinkedEdge<< endl;

		EdmundKarp ed;
		uint32_t maxFlow = ed.CalculateMaxFlow(&g, &g.Nodes[0],&g.Nodes[3]);
		//Debug::log("MaxFlow: ");
		//Debug::Instance() << "Maxflow";
		//Debug << "MaxFlow: " << maxFlow;
		//std::cout << "2" << 
		//Debug d;
		//d << "MaxFlow: " << maxFlow << std::endl;	
		Debug << "Dough" << maxFlow << "\n";
		if(maxFlow==23)
			return true;
		return false;
	}*/
	 }