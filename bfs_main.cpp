/*  
    Autor: João Paulo Moro Loureiro
    Orientador: Renato Elias Nunes de Moraes
    Disciplina: TCC II - 2018/2
    Data: 01/09/2018

*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Graph.cpp"
#include "bfs_sequencial.cpp"
#include "bfs_paralelo.cpp"
#include "tempo.cpp"

using namespace std;

/* Compilar:
    g++ bfs_main.cpp -o tree -lOpenCL -I ../../CL-1.2/

    Executar:
    ./<executavel> <kernel> <ip_form> <arquivo_leitura> <valor_procurado>
    ./tree kernel_bfs_paralelo.c 1 input_10_ip.txt 1.1.1.5

*/

//Estrutura para os dados da Árvore AVL
typedef struct tDados
{
    int index;
    int ip;
}Dados;

//Estrutura para os nós da Árvore AVL
typedef struct tNode
{
    struct tDados dados;
    struct tNode *left;
    struct tNode *right;
    int height;
}Node;

typedef struct tIp
{
    int ip_int;
    string ip_str;
}IP;

// A utility function to get height of the tree
int height(Node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}
 
// A utility function to get maximum of two integers
int max(int a, int b)
{
    return (a > b)? a : b;
}
 
/* Helper function that allocates a new node with the given key and
    NULL left and right pointers. */
 Node* newNode(Dados dados)
{
    Node* node = (Node*)
                        malloc(sizeof(Node));
    node->dados  = dados;
    node->left   = NULL;
    node->right  = NULL;
    node->height = 1;  // new node is initially added at leaf
    return(node);
}
 
// A utility function to right rotate subtree rooted with y
// See the diagram given above.
Node *rightRotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;
 
    // Perform rotation
    x->right = y;
    y->left = T2;
 
    // Update heights
    y->height = max(height(y->left), height(y->right))+1;
    x->height = max(height(x->left), height(x->right))+1;
 
    // Return new root
    return x;
}
 
// A utility function to left rotate subtree rooted with x
// See the diagram given above.
Node *leftRotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;
 
    // Perform rotation
    y->left = x;
    x->right = T2;
 
    //  Update heights
    x->height = max(height(x->left), height(x->right))+1;
    y->height = max(height(y->left), height(y->right))+1;
 
    // Return new root
    return y;
}
 
// Get Balance factor of node N
int getBalance(Node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

Dados getNode(Node *root, int key)
{
    Dados dados_null;
    //cout << key << "|" << root->dados.ip << endl;
    if (root == NULL)
    {
        return dados_null;
    }
    else if (key == root->dados.ip)
    {
        return root->dados;
    }
    else if (key < root->dados.ip)
    {
        return getNode(root->left, key);
    }
    else
    {
        return getNode(root->right, key);
    }
}
 
// Recursive function to insert key in subtree rooted
// with node and returns new root of subtree.
Node* insert(Node* node, Dados dados)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return(newNode(dados));
 
    if (dados.ip < node->dados.ip)
        node->left  = insert(node->left, dados);
    else if (dados.ip > node->dados.ip)
        node->right = insert(node->right, dados);
    else // Equal keys are not allowed in BST
        return node;
 
    /* 2. Update height of this ancestor node */
    node->height = 1 + max(height(node->left),
                           height(node->right));
 
    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);
 
    // If this node becomes unbalanced, then
    // there are 4 cases
 
    // Left Left Case
    if (balance > 1 && dados.ip < node->left->dados.ip)
        return rightRotate(node);
 
    // Right Right Case
    if (balance < -1 && dados.ip > node->right->dados.ip)
        return leftRotate(node);
 
    // Left Right Case
    if (balance > 1 && dados.ip > node->left->dados.ip)
    {
        node->left =  leftRotate(node->left);
        return rightRotate(node);
    }
 
    // Right Left Case
    if (balance < -1 && dados.ip < node->right->dados.ip)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
 
    /* return the (unchanged) node pointer */
    return node;
}

/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
Node * minValueNode(Node* node)
{
    Node* current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
Node* deleteNode(Node* root, Dados dados)
{
    // STEP 1: PERFORM STANDARD BST DELETE
 
    if (root == NULL)
        return root;
 
    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if ( dados.ip < root->dados.ip )
        root->left = deleteNode(root->left, dados);
 
    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if( dados.ip > root->dados.ip )
        root->right = deleteNode(root->right, dados);
 
    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            Node *temp = root->left ? root->left : root->right;
 
            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else // One child case
             *root = *temp; // Copy the contents of
                            // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            Node* temp = minValueNode(root->right);
 
            // Copy the inorder successor's data to this node
            root->dados.ip = temp->dados.ip;
 
            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->dados);
        }
    }
 
    // If the tree had only one node then return
    if (root == NULL)
      return root;
 
    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left),
                           height(root->right));
 
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance(root);
 
    // If this node becomes unbalanced, then there are 4 cases
 
    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
 
    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left =  leftRotate(root->left);
        return rightRotate(root);
    }
 
    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
 
    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
 
    return root;
}

// Inicializa um novo grafo
Graph criaGrafo(int qtdNos)
{
    Graph G;
    double posx,posy,normaliza;
    int i;

    G.V.resize(qtdNos);
    G.Value.resize(qtdNos);
    G.posX.resize(qtdNos);
    G.posY.resize(qtdNos);
    G.listAdj.resize(qtdNos);

    return G;
}
 
// Percorre a árvore em preorder
// Armazenando um vetor com os nós
// E preenchendo o grafo
void preOrder(Node *root, vector<Dados> lista, Graph *G)
{
    Dados dados_no;
    List elemento_G;

    if(root != NULL)
    {        
        dados_no = root->dados;

        G->V[dados_no.index] = dados_no.index;
        G->Value[dados_no.index] = dados_no.ip;

        //cout << "Vértice: (" << dados_no.index << "," << dados_no.ip << ")";
        if (root->left != NULL)
        {
            elemento_G.v = root->left->dados.index;
            elemento_G.cost = 1;
            G->listAdj[dados_no.index].push_back(elemento_G);

            //cout << " Esquerda: (" << root->left->dados.index << "," << root->left->dados.ip << ")" ;
        }
        if (root->right != NULL)
        {
            elemento_G.v = root->right->dados.index;
            elemento_G.cost = 1;
            G->listAdj[dados_no.index].push_back(elemento_G);

            //cout << " Direita: (" << root->right->dados.index << "," << root->right->dados.ip << ")" ;
        }

        //cout << endl;

        lista.push_back(dados_no);

        preOrder(root->left, lista, G);
        preOrder(root->right, lista, G);
    }
}

void escreveSaidaTree (string saida, Graph G, int root_key)
{
    fstream arquivoSaida;
    string headerListAdj = "LINKS_SOURCE_DESTINATION_DISTANCE";
    string headerCoord = "COORD_X_Y";
    int links = 0;

    arquivoSaida.open(saida.data(),ios::out);

    arquivoSaida << headerCoord << " " << G.V.size() << endl;
    for(int i=0;i<G.posX.size();i++)
        arquivoSaida << setw(9) << G.posX[i] << " " << setw(9) << G.posY[i] << endl;

    for(int i=0;i<G.listAdj.size();i++)
        links += G.listAdj[i].size();
    
    arquivoSaida << headerListAdj << " " << links << endl;
    //arquivoSaida << headerListAdj << " " << links << " " << root_key << endl; //Gravar raiz da árvore

    for(int i=0;i<G.listAdj.size();i++)
    {
        for(int j=0;j<G.listAdj[i].size();j++)
        {
            arquivoSaida << setw(6) << i << " " << setw(6) << G.listAdj[i][j].v << " " << setw(6) << G.listAdj[i][j].cost << endl;
        }
    }
    arquivoSaida.close();
}

void read_file(string file_name, std::vector<IP> &t_lines, int ip_form)
{
    fstream file;
    string line_file, line;
    const char* line_c;
    struct sockaddr_in line_ip;
    IP ip_line;

    file.open (file_name, ios::in);
    if (!file.is_open())
    {
        cout << "Erro ao carregar o arquivo";
        return;
    }

    while(getline(file, line_file))
    {
        if (ip_form == 1)
        {
            // Transformar os IPs em inteiros, para serem inseridos na árvore
            line_c = NULL;
            line.clear();

            line_c = line_file.c_str();
            ip_line.ip_str = line_file;

            //cout << line_c << endl; // IPs
            inet_aton(line_c, &line_ip.sin_addr); // get integer IP
            line = to_string(line_ip.sin_addr.s_addr);
            //cout << line << endl; // Inteiros
            ip_line.ip_int = atoi(line.c_str());
            t_lines.push_back(ip_line);
            //t_lines.push_back(line);
        }
        else
        {
            ip_line.ip_str = line_file;
            ip_line.ip_int = atoi(line_file.c_str());
            t_lines.push_back(ip_line);
        }
        
    }
}

/* Programa principal para testes
argv[1] = Nome do arquivo com os dois kernels
argv[2] = Nome do arquivo de entrada
argv[3] = Arquivo com os IPs buscados
argv[4] = Repetições da execução
argv[5] = Formato do Ip no arquivo: 1 = a.b.c.d, senão = inteiro.
*/
int main(int argc, char** argv)
{
    int i, j, k, level = 0, repeticoes;
    int root_key = 0, key_to_find, result_index;
    double inicio, fim, tempo_exec;
    string file_name_grafo = "GraphAVLTree.txt";
    Dados dados;
    Node *root = NULL;
    Graph G;
    int formato_ip;

    fstream arquivoSaida;
    //arquivoSaida.open(saida.data(),ios::out);

    vector<IP> file_data, file_ip_busca;
    vector<Dados> root_lista;

    if (argc < 4)
    {
        cout << "Informe o arquivo e o nº buscado." << endl;
        return 0;
    }

    if (argc = 5)
    {
        formato_ip = atoi(argv[5]);
    }
    else
    {
        formato_ip = 0;
    }

    repeticoes = atoi(argv[4]);

    //read_file("input.txt", file_data);
    cout << "-- Carregando Arquivo:" << endl;
    //read_file(argv[2], file_data, 1);
    read_file(argv[2], file_data, formato_ip);

    //read_file(argv[3], file_ip_busca, 1);
    read_file(argv[3], file_ip_busca, formato_ip);

    cout << "\tRegistros:" << file_data.size() << endl;
    cout << "-- Criando árvore AVL:" << endl;
    for (i = 0; i < file_data.size(); i++)
    {
        dados.index = i;
        //stringstream str(file_data.at(i).ip_int);
        //str >> dados.ip;

        dados.ip = file_data[i].ip_int;
        //cout << dados.ip << endl;
        root = insert(root, dados);
    }

    root_key = root->dados.index;

    cout << "\tRoot: (" << root_key << "," << root->dados.ip << ")" << endl;
    cout << "-- Carregando a árvore no Grafo:" << endl;
    
    G = criaGrafo(file_data.size());

    preOrder(root, root_lista, &G);

    escreveSaidaTree(file_name_grafo, G, root_key);

/*
    cout << "-- Processamento Normal:" << endl;
    arquivoSaida.open("EXECNormal.txt",ios::out);
    for(i = 0; i < file_ip_busca.size(); i++)
    {
        key_to_find = file_ip_busca[i].ip_int;
        for(j = 0; j < repeticoes; j++)
        {
            inicio = getcputime();
            for(k = 0; k < file_data.size(); k++)
            {
                if(file_data[k].ip_int == key_to_find)
                {
                    result_index = k;
                    fim = getcputime();
                    arquivoSaida << file_ip_busca[i].ip_str << "::" << (double)(fim-inicio) << "::" << result_index << endl;
                    break;
                }
            }
            if( k == file_data.size()-1)
            {
                result_index = -1;
                fim = getcputime();
                arquivoSaida << file_ip_busca[i].ip_str << "::" << (double)(fim-inicio) << "::" << result_index << endl;
            }
        }
    }
    arquivoSaida.close();
*/

    cout << "-- Processamento Sequencial:" << endl;
    arquivoSaida.open("EXECSequencial.txt",ios::out);
    for(i = 0; i < file_ip_busca.size(); i++)
    {
        for(j = 0; j < repeticoes; j++)
        {
            //key_to_find = atoi(file_ip_busca[i].ip_str.c_str());
            //cout << key_to_find << endl;
            key_to_find = file_ip_busca[i].ip_int;
            
            inicio = getcputime();
            result_index = bfs(G, root_key, key_to_find);
            fim = getcputime();
            
            //arquivoSaida << file_ip_busca[i].ip_str << "::" << tempo_exec << "::" << result_index << endl;
            arquivoSaida << file_ip_busca[i].ip_str << "::" << (double)(fim-inicio) << "::" << result_index << endl;

            //cout << "\tIndex Encontrado(sequencial): " << result_index << endl;
            //cout << "\tTempo Sequencial getcputime: " << (double)(fim-inicio) << endl << endl;
        }
    }
    arquivoSaida.close();

    cout << "-- Processamento Paralelo:" << endl;
    arquivoSaida.open("EXECParaleloGPU.txt",ios::out);
    for(i = 0; i < file_ip_busca.size(); i++)
    {
        for(j = 0; j < repeticoes; j++)
        {
            key_to_find = file_ip_busca[i].ip_int;
            
            result_index = bfs_paralelo(G, root_key, argv[1], &tempo_exec, key_to_find);
                        
            arquivoSaida << file_ip_busca[i].ip_str << "::" << tempo_exec << "::" << result_index << endl;
        }
    }
    arquivoSaida.close();


    //cout << "\tIndex Encontrado(paralelo): " << result_index << endl;
    //cout << "\tTempo Paralelo getcuptime (total): " << (double)(fim-inicio) << endl;

    return 0;
}