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
            
  
