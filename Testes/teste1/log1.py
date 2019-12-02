import sys
from collections import deque

linha=51

fila_execucao = deque([])

instrucoes_ativas = {}

def erro(msg):
    global linha
    print("Erro na linha "+str(linha))
    print(msg)
    sys.exit()

def esp(tipo, id):
    global linha

    linha+=1
    fila_execucao.append((tipo,id))
    return

def entra(tipo, id):
    global linha
    
    linha+=1
    if(len(fila_execucao) == 0):
       erro("Thread começou a executar com fila de execucao vazia")
    topo = fila_execucao[0]
    if((tipo,id) != topo):
        erro("Thread começou a executar sem ser a próxima da fila de execucao")
    fila_execucao.popleft()
   
    if((tipo,id) in instrucoes_ativas ):
        instrucoes_ativas[(tipo,id)] += 1
    else:
        instrucoes_ativas[(tipo,id)] = 0 

def sai(tipo, id):
    global linha

    linha+=1
    
    if(not((tipo,id) in instrucoes_ativas)):
        erro("Thread que não estava sendo executada saiu")
    else:
        instrucoes_ativas[(tipo,id)] -= 1 
        if(instrucoes_ativas[(tipo,id)] == 0):
            instrucoes_ativas.pop((tipo,id))
            
  
esp(0,0)
entra(0,0)
esp(0,1)
sai(0,0)
esp(0,3)
entra(0,1)
esp(0,4)
esp(1,1)
esp(0,5)
esp(1,7)
sai(0,1)
esp(0,7)
esp(1,4)
esp(1,6)
esp(1,0)
esp(0,2)
esp(1,5)
esp(1,3)
esp(0,6)
entra(0,3)
esp(1,2)
sai(0,3)
entra(0,4)
sai(0,4)
entra(1,1)
sai(1,1)
entra(0,5)
sai(0,5)
entra(1,7)
sai(1,7)
entra(0,7)
sai(0,7)
entra(1,4)
sai(1,4)
entra(1,6)
sai(1,6)
entra(1,0)
sai(1,0)
entra(0,2)
sai(0,2)
entra(1,5)
sai(1,5)
entra(1,3)
sai(1,3)
entra(0,6)
sai(0,6)
entra(1,2)
sai(1,2)

print("Ordem de execuções das threads é válida")
#Tempo total de execução: 0.0058813450
