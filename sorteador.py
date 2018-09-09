#!/usr/bin/env python
import socket
import struct
import random
import math
import sys

file_name_in = sys.argv[1]
file_name_out = file_name_in.replace(".txt","")
file_name_out = file_name_out + "_out.txt"

file_in = open (file_name_in, "r")
file_out = open (file_name_out, "w")

texto = file_in.readlines()

n_linhas = len(texto)


n_sorteio = int(sys.argv[2])

i = 0

while i < n_sorteio:
    i = i + 1
    sorteio = random.randint(1, n_sorteio);
    linha = texto[sorteio]

    linha = linha
    file_out.write(linha)
    
file_in.close()
file_out.close()