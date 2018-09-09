__kernel void kernel_1( __global int* VaB, __global int* EaB, __global int* FaB,
                        __global int* FuaB, __global int* XaB, __global int* CaB, int numVert, int numAresta,
                        __global int* result_indexB, __global int* ValuesB, int key_to_find)
{
	int inicio, fim, id, nid;
	
	id = get_global_id(0);
	if(id<numVert)
	{
		if(FaB[id] == 1)
		{
            // JPL
            // Se o valor do vértice atual for o valor buscado, retornar o index do vértice
            if(ValuesB[id] == key_to_find)
            {
                *result_indexB = id; // Retornar o index do nó
            }
            else
            {
                FaB[id] = 0;
                inicio = VaB[id];

                if(id!=numVert-1)
                    fim = VaB[id+1]-1;
                else
                    fim = numAresta-1;
            
                for(int i=inicio;i<=fim;i++)
                {
                    nid = EaB[i];
                    if(!XaB[nid])
                    {
                        CaB[nid] = CaB[id]+1;
                        FuaB[nid] = 1;
                    }
                }
            }
            
		}
	}
}

__kernel void kernel_2(__global int* FaB, __global int* FuaB, __global int* XaB, __global int* fimExecB, int numVert)
{
    int id = get_global_id(0);

	if(id<numVert)
	{
        if(FuaB[id])
		{
			FaB[id] = 1;
			XaB[id] = 1;
			FuaB[id] = 0;
            *fimExecB = 0;
		}
	}
}
