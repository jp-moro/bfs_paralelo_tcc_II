import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick

#dataseq = np.loadtxt('normalMedia.txt')
#datagpu = np.loadtxt('opencl-nvidiaMedia.txt')

#datanor = np.loadtxt('NormalMedia.txt')
#datagpu = np.loadtxt('ParaleloGPUMedia.txt')
#dataseq = np.loadtxt('SequencialMedia.txt')

#datanor = np.loadtxt('MEDIAEXECNormal.txt')
datagpu = np.loadtxt('MEDIAEXECParaleloGPU.txt')
dataseq = np.loadtxt('MEDIAEXECSequencial.txt')

#sorted_datanor = np.sort(datanor)
sorted_dataseq = np.sort(dataseq)
sorted_datagpu = np.sort(datagpu)

#yvalsnor=np.arange(len(sorted_datanor))/float(len(sorted_datanor)-1)
yvalsseq=np.arange(len(sorted_dataseq))/float(len(sorted_dataseq)-1)
yvalsgpu=np.arange(len(sorted_datagpu))/float(len(sorted_datagpu)-1)

#plt.plot(sorted_dataseq,yvalsseq,sorted_datanor,yvalsnor,sorted_datagpu,yvalsgpu)
#plt.legend(['BFS Sequencial(CPU)', 'Consulta Direta(CPU)', 'BFS OpenCL(GPU)'], loc='best')

plt.plot(sorted_dataseq,yvalsseq,sorted_datagpu,yvalsgpu)
plt.legend(['BFS Sequencial(CPU)', 'BFS OpenCL(GPU)'], loc='best') #JPL
plt.title("Consulta IP")
plt.ylim((0, 1))
plt.ylabel("CDF")
plt.xlabel("Tempo de Consulta (s)") # JPL
plt.grid(True)

plt.ticklabel_format(style='sci', axis='x', scilimits=(0,0))

plt.show()
