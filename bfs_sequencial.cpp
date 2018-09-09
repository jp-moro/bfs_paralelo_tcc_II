#include "Graph.cpp"
#include "tempo.cpp"

/*
G = Grafo de entrada
s = nó inicial do grafo (Raiz da Árvore)
key = chave buscada no grafo
*/

int bfs (Graph G, int s, int key_to_find)
{
    Graph solucao = G;
    vector<int> Q;
    vector<int> color, pi, d;
    int u,v;
    int fim_exec = 0;
    int count = 0, i = 0;

    color.resize(G.V.size(),0);
    pi.resize(G.V.size(),-1);
    d.resize(G.V.size(),-1);

    color[s]=1;
    d[s]=0;
    pi[s]=-1;

/*    
    //Percorrer a lista de adjacencias, e exibir as conexões de cada vértice
    //cout << endl << "Adjacent List:" << endl;

    for(int k = 0; k < G.listAdj.size(); k++)
    {
        //Vértice "Cabeça" da lista (índice,valor)
        cout << "Vertice: (" << k << "," << G.Value[k] << ") -> ";
        for(int l = 0; l < G.listAdj[k].size(); l++)
        {
            //Vértices conectados à este (índice,valor)
            cout << "(" << G.listAdj[k][l].v << "," << G.Value[G.listAdj[k][l].v] << ")";
        }
        cout << endl;
    }
*/    
    
    Q.push_back(s);
    //cout << endl << "PUSH: " << s << endl;

    for(int i=0;i<Q.size();i++)
    //while (Q.size() != 0 && fim_exec != 1)
    //while(i < Q.size() && fim_exec != 1)
    {
        //u = Q[Q.size()-1];
        
        //Q.pop_back();
        u = Q[i];
        //cout << "POP: " << u << endl;

        //O valor buscado pode estar no vértice raiz
        if (G.Value[u] == key_to_find)
        {
            fim_exec = 1;
            return u;
            //cout << endl << key_to_find << " Found. Index: " << u << endl;
            //break;
        }
        
        for(int j=0;j<G.listAdj[u].size();j++)
        {
            count ++;
            v = G.listAdj[u][j].v;
            if(color[v]==0)
            {
                color[v]=1;
                d[v]=d[u]+1;
                pi[v]=u;
                //cout << "V: " << v << endl;
                Q.push_back(v);
                //cout << "PUSH: " << v << endl;  
            }
            //Verificar valor do vértice a cada iteração
            if (G.Value[v] == key_to_find)
            {
                fim_exec = 1;
                //cout << endl << key << " Found. Index: " << v << endl;
                return v;
                //break;
            }
        }
        
        color[u]=2;
    }

    //if (fim_exec != 1) cout << endl << key << " Not found" << endl;

    //cout << "Iterações: " << count << endl;
    return -1;
}
/*
main(int argc, char** argv)
{
    Graph grafo;
    double inicio, fim;
    int key;

    grafo = readGraph(argv[1]);
    key = atoi(argv[2]);

    inicio = getcputime();
    bfs(grafo, 0, key);
    fim = getcputime();
    
    //cout << grafo.V.size() << " " << (double)(fim-inicio) << endl;
}
*/
