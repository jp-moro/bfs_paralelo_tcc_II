#/bin/bash

#    Autor: João Paulo Moro Loureiro
#    Orientador: Renato Elias Nunes de Moraes
#    Disciplina: TCC II - 2018/2
#    Data: 01/09/2018

# $1 = Lista de IPs (Tabela de encaminhamento)
# $2 = IPs a serem buscados
# $3 = Formato do IP (se 1 = a.b.c.d senão, inteiro)

REPEAT=3

INPUT_LISTA=$1
INPUT_BUSCA=$2

# Compilar executável
g++ bfs_main.cpp -o bfs_main -lOpenCL -I ../../CL-1.2

# Executar BFS sequencial e paralelo
./bfs_main kernel_bfs_parapelo.c $INPUT_LISTA $INPUT_BUSCA $REPEAT $3

# Compilar resultados
python compileresults.py EXECParaleloGPU.txt $REPEAT
python compileresults.py EXECSequencial.txt $REPEAT

# Plotar Gráfico
python3 generateplot.py

