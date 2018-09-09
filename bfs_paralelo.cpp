#include <CL/opencl.h>
#include <CL/cl.h>
#include <CL/cl.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <string>
#include <fstream>
#include <stdio.h>

#include <time.h>
#include <sys/time.h>

#include "Graph.cpp"
#include "tempo.cpp"
using namespace std;

// g++ ParallelBFS_GPU_AvlTree.cpp -o paralelo -lOpenCL -I ../../CL-1.2/

string carregaKernel(string entrada)
{
    string kernel, linha;
    ifstream arquivo;

    kernel.clear();
    linha.clear();

    arquivo.open(entrada.data());
    while (!arquivo.eof())
    {
        getline(arquivo, linha);
        linha += "\n";
        kernel += linha;
    }
    arquivo.close();

    return kernel;
}

int bfs_paralelo(Graph G, int s, string file_kernel, double *tempo_exec, int key_to_find)
{
    // Variaveis OpenCL:
    //---------------------------------------------------------
    vector<cl::Platform> plataformas;
    vector<cl::Device> dispositivos;
    cl::Context contexto;
    cl::CommandQueue listaComandos;
    cl::Program::Sources codigoFonte;
    cl::Program programa;
    cl::Kernel kernel1, kernel2;
    cl::Buffer VaB, ValuesB, EaB, FaB, FuaB, XaB, CaB, fimExecB, result_indexB;
    cl::Buffer bufferIn;
    cl::Event evento1, evento2;
    cl_ulong inicio = (cl_ulong)0;
    cl_ulong fim = (cl_ulong)0;
    //---------------------------------------------------------

    //Variaveis de controle/manipulacao BFS:
    //---------------------------------------------------------
    //Graph G;
    int numVert, numAresta = 0;
    float tempoTotal = 0, tempoParcial;
    string kernel_str;
    int *Va, *Values, *Ea, *Fa, *Fua, *Xa, *Ca, *fimExec, k = 0;
    int *result_index;

    double tempoInicio = 0, tempoFim = 0;
    clock_t t;
    struct timeval time1, time2;
    //---------------------------------------------------------

    //G = readGraph(argv[1]);

    numVert = G.V.size();

    for (int i = 0; i < G.listAdj.size(); i++)
        numAresta += G.listAdj[i].size();

    Va = new int[numVert];
    Values = new int[numVert];
    Ea = new int[numAresta];
    Fa = new int[numVert];
    Fua = new int[numVert];
    Xa = new int[numVert];
    Ca = new int[numVert];
    fimExec = new int(0);
    result_index = new int(0);

    for (int i = 0; i < G.listAdj.size(); i++)
    {
        //Va[i] = k;
        //Values[i] = G.Value[i]; // JPL
        //cout << "Index: " << G.V[i] << "Valor: " << G.Value[G.V[i]] << endl;
        Va[i] = k;
        Values[i] = G.Value[G.V[i]];
        //cout << "Index: " << Va[i] << "Valor: " << Values[i] << endl;
        if (G.listAdj[i].size() == 0) // JPL
        {
            //Ea[i] = -1;
        }
        else
        {
            for (int j = 0; j < G.listAdj[i].size(); j++)
            {
                Ea[k] = G.listAdj[i][j].v;
                k++;
            }
        }
    }
    /*
 G.V.clear();
 G.posX.clear();
 G.posY.clear();
 G.sinr.clear();
 G.power.clear();
 G.listAdj.clear();
 */
    for (int i = 0; i < numVert; i++)
    {
        Fa[i] = 0;
        Fua[i] = 0;
        Ca[i] = 0;
        Xa[i] = 0;
    }
    // Fa[0] = 1;
    // Xa[0] = 1;
    Fa[s] = 1; //Inicializar para começar a buscar do nó raiz do grafo
    Xa[s] = 1;

    // Teste, exibir a tabela de Vértices e Frontier
    /*
for (int i=0;i<numVert; i++)
{
    cout << "|" << Va[i];
}

cout << endl;
for (int i=0;i<numVert; i++)
{
    cout << "|" << Fa[i];
}

cout << endl;
for (int i=0; i< numAresta; i++)
{
    cout << "|" << Ea[i];
}

cout << endl;
*/

    //kernel_str = carregaKernel(argv[2]);
    kernel_str = carregaKernel(file_kernel);
    // cout << kernel_str << endl;

    //Descobrir quais plataformas estão instaladas na maquina.
    cl::Platform::get(&plataformas);
    //cout << "Plataformas: " << plataformas.size() << endl;

    string nomeDispositivo;
    string nomePlataforma;
    string versaoPlataforma;

    //Descobrir quais dispositivos estão instalados.
    plataformas[0].getDevices(CL_DEVICE_TYPE_ALL, &dispositivos);
    // plataformas[0].getInfo(CL_PLATFORM_NAME, &nomePlataforma);
    // plataformas[0].getInfo(CL_PLATFORM_VERSION, &versaoPlataforma);
    //cout << "Dispositivos: " << dispositivos.size() << endl;

    //dispositivos[0].getInfo(CL_DEVICE_NAME, &nomeDispositivo);

    //cout << "nome da plataforma: " << nomePlataforma << endl;
    //cout << "versao da plataforma: " << versaoPlataforma << endl;
    //cout << "nome do dispositivo: " << nomeDispositivo << endl << endl;

    //Criar contexto.
    contexto = cl::Context(dispositivos);

    //Criar lista de comandos para o(s) dispositivo(s).
    listaComandos = cl::CommandQueue(contexto, dispositivos[0], CL_QUEUE_PROFILING_ENABLE);

    //Carregar o programa.
    codigoFonte = cl::Program::Sources(1, make_pair(kernel_str.data(), kernel_str.size()));
    programa = cl::Program(contexto, codigoFonte);

    //Compilar em todos os dispositivos, o codigo fonte gerado acima.
    programa.build(NULL);

    //Criando os kernels.
    kernel1 = cl::Kernel(programa, "kernel_1", NULL);
    kernel2 = cl::Kernel(programa, "kernel_2", NULL);

    //Criando buffers para os vetores:
    VaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    ValuesB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    EaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numAresta * sizeof(int));
    FuaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    FaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    XaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    CaB = cl::Buffer(contexto, CL_MEM_READ_WRITE, numVert * sizeof(int));
    fimExecB = cl::Buffer(contexto, CL_MEM_READ_WRITE, sizeof(int));
    result_indexB = cl::Buffer(contexto, CL_MEM_READ_WRITE, sizeof(int));

    //Transferindo dados para o buffer de entrada.
    listaComandos.enqueueWriteBuffer(VaB, CL_TRUE, 0, numVert * sizeof(int), Va);
    listaComandos.enqueueWriteBuffer(ValuesB, CL_TRUE, 0, numVert * sizeof(int), Values);
    listaComandos.enqueueWriteBuffer(EaB, CL_TRUE, 0, numAresta * sizeof(int), Ea);
    listaComandos.enqueueWriteBuffer(FaB, CL_TRUE, 0, numVert * sizeof(int), Fa);
    listaComandos.enqueueWriteBuffer(FuaB, CL_TRUE, 0, numVert * sizeof(int), Fua);
    listaComandos.enqueueWriteBuffer(XaB, CL_TRUE, 0, numVert * sizeof(int), Xa);
    listaComandos.enqueueWriteBuffer(CaB, CL_TRUE, 0, numVert * sizeof(int), Ca);
    listaComandos.enqueueWriteBuffer(fimExecB, CL_TRUE, 0, sizeof(int), fimExec);
    //listaComandos.enqueueWriteBuffer(result_indexB,CL_TRUE,0,sizeof(int),result_index);

    //Definindo argumentos para os kernels.
    kernel1.setArg(0, VaB);
    kernel1.setArg(1, EaB);
    kernel1.setArg(2, FaB);
    kernel1.setArg(3, FuaB);
    kernel1.setArg(4, XaB);
    kernel1.setArg(5, CaB);
    kernel1.setArg(6, sizeof(int), &numVert);
    kernel1.setArg(7, sizeof(int), &numAresta);
    kernel1.setArg(8, result_indexB);
    kernel1.setArg(9, ValuesB);                    // JPL
    kernel1.setArg(10, sizeof(int), &key_to_find); //JPL
    kernel2.setArg(0, FaB);
    kernel2.setArg(1, FuaB);
    kernel2.setArg(2, XaB);
    kernel2.setArg(3, fimExecB);
    kernel2.setArg(4, sizeof(int), &numVert);

    //Execução dos kernels.
    tempoInicio = getcputime();
    t = clock();
    gettimeofday(&time1, NULL);

    while (!*fimExec)
    //while (*fimExec != 0)
    {

        *fimExec = -1;
        *result_index = -1;

        listaComandos.enqueueWriteBuffer(fimExecB, CL_TRUE, 0, sizeof(int), fimExec);
        listaComandos.enqueueWriteBuffer(result_indexB, CL_TRUE, 0, sizeof(int), result_index);

        listaComandos.enqueueNDRangeKernel(kernel1, cl::NDRange(), cl::NDRange(numVert), cl::NDRange(), NULL, &evento1);

        listaComandos.enqueueReadBuffer(result_indexB, CL_TRUE, 0, sizeof(int), result_index);

        if (*result_index != -1)
        {
            listaComandos.enqueueReadBuffer(fimExecB, CL_TRUE, 0, sizeof(int), fimExec);
            *fimExec = 0;
            break;
        }
        else
        {
            listaComandos.enqueueNDRangeKernel(kernel2, cl::NDRange(), cl::NDRange(numVert), cl::NDRange(), NULL, &evento2);

            listaComandos.finish();

            listaComandos.enqueueReadBuffer(fimExecB, CL_TRUE, 0, sizeof(int), fimExec);
        }

        /*
	listaComandos.enqueueReadBuffer(CaB,CL_TRUE,0,numVert*sizeof(int),Ca);
	listaComandos.enqueueReadBuffer(XaB,CL_TRUE,0,numVert*sizeof(int),Xa);
	listaComandos.enqueueReadBuffer(FaB,CL_TRUE,0,numVert*sizeof(int),Fa);
	listaComandos.enqueueReadBuffer(FuaB,CL_TRUE,0,numVert*sizeof(int),Fua);
	listaComandos.enqueueReadBuffer(fimExecB,CL_TRUE,0,sizeof(int),fimExec);
	cout << "Resultado Parcial: " << endl;
	cout << "Xa:  ";
	for(int i=0;i<numVert;i++)
	 cout << "(" << i << ";" << Xa[i] << ") ";
	cout << endl;
	cout << "Ca:  ";
	for(int i=0;i<numVert;i++)
	 cout << "(" << i << ";" << Ca[i] << ") ";
	cout << endl;
	cout << "Fa:  ";
	for(int i=0;i<numVert;i++)
	 cout << "(" << i << ";" << Fa[i] << ") ";
	cout << endl;
	cout << "Fua: ";
	for(int i=0;i<numVert;i++)
	 cout << "(" << i << ";" << Fua[i] << ") ";
	cout << endl;
	cout << "fimExec: " << *fimExec << endl;
*/

        clGetEventProfilingInfo(evento1(), CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &inicio, NULL);
        clGetEventProfilingInfo(evento2(), CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &fim, NULL);
        tempoParcial = (float)(fim - inicio) / 1e9;
        //	cout << "Tempo parcial de execucao: " << tempoParcial << endl;
        tempoTotal += tempoParcial;
    }
    *tempo_exec = tempoTotal;

    tempoFim = getcputime();

    t = clock() - t;
    gettimeofday(&time2, NULL);
    //cout << numVert << " " << tempoTotal << endl;

    //cout << "Tempo medido pelo profiler OpenCL: " << tempoTotal << endl;

    //cout << "\tTempo medido usando getcputime(): " << (double)(tempoFim - tempoInicio) << endl;
    //cout << "Tempo medido usando clock(): " << t << " clicks, " << ((double)t)/CLOCKS_PER_SEC << endl;
    //cout << "Tempo medido usando gettimeofday() (usec): " << (double)(time2.tv_usec-time1.tv_usec) << endl;
    //cout << "Tempo medido usando gettimeofday() (sec): " << (double)(time2.tv_sec-time1.tv_sec) << endl;

    // Desalocar ponteiros que contém os vetores
    delete Va;
    delete Values;
    delete Ea;
    delete Fa;
    delete Fua;
    delete Xa;
    delete Ca;

    if (*result_index != -1)
    {
        //cout << "Encontrado: " << *result_index << endl;
        return *result_index;
    }
    else
    {
        return *fimExec;
    }
}