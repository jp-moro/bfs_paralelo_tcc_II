#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <limits.h>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string.h>
#include <sstream>
#define PI 3.141592654

using namespace std;

typedef struct tList
{
 int v;
 double cost;
}List;

typedef struct tgraph
{
 vector<vector<List> > listAdj;
 vector<int> V;
 vector<int> Value;
 vector<double> posX,posY;
 vector<double> sinr;
 vector<double> power;
}Graph;

Graph readGraph(char* arq)
{
  ifstream fileInput;
  Graph G;
  int numVert, links, origin, destiny;
  List element;
  double cost;
  string stringLine;
  
  fileInput.open(arq,ios::in);

  fileInput >> stringLine;
  while(stringLine.compare("COORD_X_Y")!=0)
   fileInput >> stringLine;

  fileInput >> numVert;
  
  G.V.resize(numVert);
  G.listAdj.resize(numVert);
  
  for(int i=0;i<numVert;i++)
   G.V[i] = i;
  
  fileInput >> stringLine;
  while(stringLine.compare("LINKS_SOURCE_DESTINATION_DISTANCE")!=0)
   fileInput >> stringLine;
   
  fileInput >> links;  
  for(int i=0;i<links;i++)
  {
	fileInput >> origin;
	fileInput >> destiny;
	fileInput >> cost;
	
	element.v = destiny;
	element.cost = cost;
	G.listAdj[origin].push_back(element);
  }
  
  fileInput.close();
    
  return G;
}

void writeDot(char *arq, Graph G)
{
 ofstream fileOut;
 fileOut.open(arq,ofstream::out);
 int u,v;
 double cost;
 vector<int> discovered;
 
 discovered.resize(G.V.size(),0);
 
 fileOut << "graph G {" << endl;
 fileOut << "size=\"8.5,11;\"" << endl;
 fileOut << "ratio = \"expand;\"" << endl;
 fileOut << "fixedsize=\"true;\"" << endl;
 fileOut << "overlap=\"scale;\"" << endl;
 fileOut << "node[shape=circle,width=.12,hight=.12,fontsize=12]" << endl;
 fileOut << "edge[fontsize=12]" << endl;
 fileOut << endl;
  
 for(int i=0;i<G.V.size();i++)
 {
  fileOut << G.V[i] << "[label=\" " << G.V[i] << "\"";
  fileOut << " color=black, pos=\"";
  fileOut << setprecision(8) << G.posX[G.V[i]] << "," << G.posY[G.V[i]];
  fileOut << "!\"];" << endl;
 }
 
 for(int i=0;i<G.listAdj.size();i++)
 {
  for(int j=0;j<G.listAdj[i].size();j++)
  {
   u = i;
   v = G.listAdj[i][j].v;
   cost = G.listAdj[i][j].cost;
   
   if(discovered[v]==0)
   {
    fileOut << u << "--" << v << "[label=\"";
    fileOut << setprecision(3) << cost << "\"]" << endl;
   }
   discovered[u]=1;
  }
 }
 
 
 fileOut << endl;
 
 
 fileOut << "}" << endl;
 fileOut.close();
}

void graphOutput(char *arq, Graph G)
{
 string comandSys;
 
 comandSys = "neato -s -n -Tps ";
 comandSys += arq;
 comandSys += " -o ";
 comandSys += arq;
 comandSys += ".ps";
 writeDot(arq, G);
 
 system(comandSys.data());
 
}
#endif
