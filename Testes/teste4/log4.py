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
entra(0,1)
esp(0,2)
esp(0,3)
esp(1,0)
sai(0,1)
esp(0,0)
esp(1,3)
esp(1,2)
esp(1,1)
esp(0,1)
entra(0,2)
sai(0,2)
entra(0,3)
sai(0,3)
esp(0,2)
entra(1,0)
sai(1,0)
esp(0,3)
entra(0,0)
esp(1,0)
sai(0,0)
esp(0,0)
entra(1,3)
sai(1,3)
entra(1,2)
esp(1,3)
sai(1,2)
esp(1,2)
entra(1,1)
sai(1,1)
esp(1,1)
entra(0,1)
sai(0,1)
entra(0,2)
esp(0,1)
sai(0,2)
entra(0,3)
esp(0,2)
sai(0,3)
entra(1,0)
esp(0,3)
sai(1,0)
esp(1,0)
entra(0,0)
sai(0,0)
entra(1,3)
esp(0,0)
sai(1,3)
esp(1,3)
entra(1,2)
sai(1,2)
esp(1,2)
entra(1,1)
sai(1,1)
esp(1,1)
entra(0,1)
sai(0,1)
entra(0,2)
esp(0,1)
sai(0,2)
entra(0,3)
esp(0,2)
sai(0,3)
esp(0,3)
entra(1,0)
sai(1,0)
esp(1,0)
entra(0,0)
sai(0,0)
entra(1,3)
sai(1,3)
esp(1,3)
entra(1,2)
sai(1,2)
esp(1,2)
entra(1,1)
sai(1,1)
esp(1,1)
entra(0,1)
sai(0,1)
entra(0,2)
sai(0,2)
entra(0,3)
sai(0,3)
entra(1,0)
sai(1,0)
entra(1,3)
sai(1,3)
entra(1,2)
sai(1,2)
entra(1,1)
sai(1,1)

print("Ordem de execuções das threads é válida")
#Tempo total de execução: 0.0063599660
