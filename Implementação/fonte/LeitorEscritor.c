#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"


enum tipos {LEITOR, ESCRITOR};
enum acoes {ESPERANDO, ENTROU, SAIU};


int conteudo;

int leitores_ativos, escritores_ativos;

int num_l, num_e;

int senha_atual=0;

pthread_mutex_t mutex, mutexfila;

pthread_cond_t condl, conde, condfila;

char arquivolog[256];


int valida_arquivo_log();
void inicializa_log();
void gera_log(int tipo, int id, int acao);

void finaliza_log(double tempo_total);

int get_senha(int tipo, int id);
void registra_leitura(int id, int conteudo);

void entra_fila(int tipo, int id, int senha);
void sai_fila();

void entra_l(int id, int senha);
void sai_l(int id);

void entra_e(int id, int senha);
void sai_e(int id);

void * ler(void *threadid);
void * escrever(void *threadid);

int main(int agrc, char *argv[]){
	double tempo_ini, tempo_fim;
	GET_TIME(tempo_ini);
		
	int num_threads_l, num_threads_e;
	
	pthread_t *tidl, *tide;
	int *idl, *ide;
	
	
	printf("Numero de threads leitoras: " );
	scanf("%d", &num_threads_l);

	printf("Numero de threads escritoras: ");
	scanf("%d", &num_threads_e);

	printf("Numero de leituras: ");
	scanf("%d", &num_l);

	printf("Numero de escritas: ");
	scanf("%d", &num_e);
	
	printf("Nome do arquivo de log(.py): ");
	scanf("%s",arquivolog);
	
	if(!valida_arquivo_log()){
		printf("Nome invalido para arquivo de log(usar extensao .py)\n"); exit(-1);
	}
	
	tidl = malloc(num_threads_l*sizeof(pthread_t));
	if(tidl == NULL){
		printf("Erro na alocação de memória de tidl\n"); exit(-1);
	}

	tide = malloc(num_threads_e*sizeof(pthread_t));
	if(tide == NULL){
		printf("Erro na alocação de memória de tide\n"); exit(-1);
	}

	idl = malloc(num_threads_l*sizeof(int));
	if(idl == NULL){
		printf("Erro na alocação de memória de idl\n"); exit(-1);
	}

	ide = malloc(num_threads_e*sizeof(int));
	if(ide == NULL){
		printf("Erro na alocação de memória de ide\n"); exit(-1);
	}
	
	inicializa_log();

	pthread_mutex_init(&mutex, NULL); 
	pthread_mutex_init(&mutexfila, NULL);
	pthread_cond_init(&condl,NULL);
 	pthread_cond_init(&conde,NULL);
	pthread_cond_init(&condfila,NULL);

	for(int t=0;t<num_threads_l;t++){
		idl[t]=t;
		if(pthread_create(&tidl[t], NULL, ler, (void*) &idl[t])){
			printf("ERRO: criação da thread\n");exit(-1);
		}
	}
	
	for(int t=0;t<num_threads_e;t++){
		ide[t]=t;
		if(pthread_create(&tide[t], NULL, escrever, (void*) &ide[t])){
			printf("ERRO: criação da thread\n");exit(-1);
		}
	}

	for(int t=0;t<num_threads_l;t++){
		if(pthread_join(tidl[t], NULL)){
			printf("ERRO: pthread join\n");exit(-1);
		}
	}
	for(int t=0;t<num_threads_e;t++){
		if(pthread_join(tide[t], NULL)){
			printf("ERRO: pthread join\n");exit(-1);
		}
	}
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutexfila);
	pthread_cond_destroy(&condl);
	pthread_cond_destroy(&conde);
	pthread_cond_destroy(&condfila);
	
	GET_TIME(tempo_fim);
	finaliza_log(tempo_fim-tempo_ini);
	
	pthread_exit(NULL);
	
}


int valida_arquivo_log(){
	int tamanho = strlen(arquivolog);
	if(tamanho<4)
		return 0;
	if(arquivolog[tamanho-3]!='.')
		return 0;
	if(arquivolog[tamanho-2]!='p')
		return 0;
	if(arquivolog[tamanho-1]!='y')
		return 0;
	return 1;
}

void inicializa_log(){
	FILE *ptrlog, *ptrlogfunc;
	
	
	ptrlogfunc = fopen("logtemplate/template.py","r");
	if(ptrlogfunc == NULL){
		printf("Erro pra abrir arquivo com funcoes de log\n");
		exit(-1);
	}
	
	ptrlog = fopen(arquivolog,"w");
	if(ptrlog == NULL){
		printf("Erro pra abrir arquivo de log\n");
		exit(-1);
	}
	
	char c = fgetc(ptrlogfunc);
	while(c != EOF){
		fputc(c,ptrlog);
		c = fgetc(ptrlogfunc);
	}
	
	fclose(ptrlogfunc);
	fclose(ptrlog);
}


void gera_log(int tipo, int id, int acao){

	const char* funcao[] = {"esp", "entra", "sai"};

	FILE *logptr = fopen(arquivolog,"a");
	
	if(logptr == NULL){
		printf("Erro pra abrir arquivo de log\n");
		exit(-1);
	}

	fprintf(logptr,"%s(%d,%d)\n",funcao[acao],tipo,id);
	
	fclose(logptr);
}


void finaliza_log(double tempo_total){
	FILE *logptr = fopen(arquivolog,"a");
	if(logptr == NULL){
		printf("Erro pra abrir arquivo de log\n");
		exit(-1);
	}

	fprintf(logptr,"\nprint(\"Ordem de execuções das threads é válida\")\n");
	fprintf(logptr,"#Tempo total de execução: %.10lf\n",tempo_total);
	
	fclose(logptr);
}

int get_senha(int tipo, int id){
	static int senha=0;
	int senha_local;
	
	pthread_mutex_lock(&mutexfila);
	gera_log(tipo,id,ESPERANDO);
	senha_local = senha++;
	pthread_mutex_unlock(&mutexfila);
	
	return senha_local;
}



void registra_leitura(int id, int conteudo){

	char nome_arquivo[61];	
	sprintf(nome_arquivo,"leituras/%d.txt",id);

	FILE *leptr = fopen(nome_arquivo, "a");
	if(leptr == NULL){
		printf("Erro pra abrir arquivo para registrar leitura\n");
		exit(-1);
	}
	
	fprintf(leptr,"%d\n",conteudo);

	fclose(leptr);
}


void entra_fila(int tipo, int id, int senha){
	pthread_mutex_lock(&mutexfila);
	while(senha_atual!=senha){

		pthread_cond_wait(&condfila,&mutexfila);
	}
	pthread_mutex_unlock(&mutexfila);
}

void sai_fila(){
	pthread_mutex_lock(&mutexfila);
	senha_atual++;
	pthread_cond_broadcast(&condfila);
	pthread_mutex_unlock(&mutexfila);
}

void entra_l(int id, int senha){
	entra_fila(LEITOR, id, senha);

	pthread_mutex_lock(&mutex);
	
	while(escritores_ativos){

		pthread_cond_wait(&condl,&mutex);
	}
	leitores_ativos++;
	gera_log(LEITOR,id,ENTROU);

	pthread_mutex_unlock(&mutex);

	sai_fila();
}

void sai_l(int id){
	pthread_mutex_lock(&mutex);

	gera_log(LEITOR,id,SAIU);

	if(--leitores_ativos==0){
		pthread_cond_signal(&conde);
	}

	pthread_mutex_unlock(&mutex);
}


void entra_e(int id, int senha){
	

	entra_fila(ESCRITOR, id, senha);

	pthread_mutex_lock(&mutex);
	while(escritores_ativos || leitores_ativos ){
		pthread_cond_wait(&conde, &mutex);
	}
	escritores_ativos++;
	gera_log(ESCRITOR,id,ENTROU);

	pthread_mutex_unlock(&mutex);
	
	sai_fila();
}

void sai_e(int id){
	pthread_mutex_lock(&mutex);
	escritores_ativos--;
	gera_log(ESCRITOR,id,SAIU);
	pthread_cond_signal(&conde);
	pthread_cond_broadcast(&condl);


	pthread_mutex_unlock(&mutex);
}

void * ler(void *threadid){
	int id = *(int *)threadid;
	
	for(int i=0;i<num_l;i++){
		int conteudo_local;
		int senha = get_senha(LEITOR,id);
	
		entra_l(id, senha);
		
		conteudo_local = conteudo;	
		
		


		sai_l(id);



		registra_leitura(id, conteudo_local);
	}	
 
	pthread_exit(NULL);

}

void * escrever(void *threadid){
	int id = *(int *)threadid;

	for(int i=0;i<num_e;i++){
		int senha = get_senha(ESCRITOR,id);

		entra_e(id, senha);

		conteudo=id;

		sai_e(id);
	}
	pthread_exit(NULL);
}
