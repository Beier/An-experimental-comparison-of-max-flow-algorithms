#include "Graph.h"
#include <fstream>
#include <iostream>

#include <stdio.h>  /* defines FILENAME_MAX */

#include <direct.h>
#include <sstream>
#define GetCurrentDir _getcwd

using namespace std;


void Graph::SaveJpg(const char* filename)
{
	ofstream writer;
	char tempFile[255];
	GetCurrentDir(tempFile, 255*sizeof(char));
	strcat_s(tempFile, "\\temp.dot");
	writer.open(tempFile);
	writer << "digraph G {";
	cout<<endl; 
	for (uint32_t i = 0; i < NodeCount; i++)
	{
		if (i != 0)
			writer << ';';
		writer << i + 1;
	}
	for (uint32_t i = 0; i < NodeCount; i++)
	{
		for (uint32_t j = 0; j < Nodes[i].EdgeCount; j++)
		{
				writer << ';' << i + 1 << "->" << Nodes[i].Edges[j].TargetNode - Nodes + 1 << "[label=" << Nodes[i].Edges[j].Capasity;
				if (Nodes[i].Edges[j].Capasity == 0)
				{
					writer << ",style=invis";
				}
					writer << "]";
		}
	}
	writer<<'}';
	writer.flush();
	writer.close();

	std::stringstream ss;
	ss << ".\\..\\Graphvis2.30\\bin\\dot.exe -Tjpg temp.dot -o " << filename;

	int r = system(ss.str().c_str());
}

