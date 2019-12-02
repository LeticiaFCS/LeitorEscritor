# LeitorEscritor
Trabalho de implementação realizado para a disciplina Computação Concorrente(2019.2) da UFRJ com o objetivo de simular um problema clássico de concorrência (Leitor/Escritor) com ausência de inanição e gerando log de saída.

#Para compilar
• Abrir a pasta "Implementação" no terminal
• Chamar o comando "make" no terminal

#Para executar
• rm -rf leituras/* 
  (comando opcional para garantir que a pasta que registra as leituras está vazia)
• Chamar o comando "./executavel/LeitorEscritor.out" no terminal
• Digitar os dados que o programa pedir

#Saída
Após a execução do programa as leituras realizadas estarão registradas em arquivos na pasta "leituras", um para cada thread leitora no formato [número da thread].txt

#Log
Para testar o arquivo de log, após a execução do programa principal:
• Abrir a pasta "Implementação" no terminal
• Chamar o comando "python3 [nome do arquivo de log]" no terminal
Caso as threads tenham sido executadas em uma ordem válida, garantindo a inanição, será mostra a mensagem "Ordem de execuções das threads é válida". Caso o contrário será mostrada uma mensagem indicando qual o erro e em qual linha do arquivo de log ele foi detectado.

