import numpy as np

#vetor = np.array(range(1000), dtype=np.int32)

n = int(input("Quantidade de números: "))
vetor = np.random.randint(1,n,n,np.int32)

file = open ("input.txt", "w")

for i in range(len(vetor)):
    line = ''
    line = str(vetor[i])
    line = line + '\n'
    file.write(line)

file.close()