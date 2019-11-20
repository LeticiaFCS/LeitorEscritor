#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>


#define debug 


enum tipos {LEITOR, ESCRITOR};
enum acoes {ESPERANDO, ENTROU, SAIU, LEU};


int conteudo;

int leitores_ativos, escritores_ativos;

int num_l, num_e;

int senha_atual=0;

pthread_mutex_t mutex, mutexfila;

pthread_cond_t condl, conde, condfila;


void inicializa_log(){
	FILE *ptrlog, *ptrlogfunc;
	
	ptrlogfunc = fopen("log/logfunc.py","r");
	if(ptrlogfunc == NULL){
		printf("Erro pra abrir arquivo com funcoes de log\n");
		exit(-1);
	}
	
	ptrlog = fopen("log.py","w");
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
	//if(acao == ESPERANDO)return;
	const char* funcao[] = {"esp", "entra", "sai"};

	FILE *logptr = fopen("log.py","a");
	
	if(logptr == NULL){
		printf("Erro pra abrir arquivo de log\n");
		exit(-1);
	}

	fprintf(logptr,"%s(%d,%d)\n",funcao[acao],tipo,id);
	
	debug("%s %d %s\n",tipo?"escritor":"leitor",id,funcao[acao]);
	fclose(logptr);
}

void gera_log_leitura(int id, int conteudo){
	return;
	FILE *logptr = fopen("log.py","a");
	if(logptr == NULL){
		printf("Erro pra abrir arquivo de log\n");
		exit(-1);
	}

	fprintf(logptr,"%s(%d,%d)\n","leu",id,conteudo);
	
	debug("%s %d %s %d\n","leitor",id,"leu",conteudo);
	fclose(logptr);

}


int get_senha(int tipo, int id){
	static int senha=0;
	int senha_local;
	
	pthread_mutex_lock(&mutexfila);
	gera_log(tipo,id,ESPERANDO);
	senha_local = senha++;
	pthread_mutex_unlock(&mutexfila);
	
	debug("\nSENHA: %d\n",senha);
	return senha_local;
}



void registra_leitura(int id, int conteudo){

	char nome_arquivo[61];	
	sprintf(nome_arquivo,"leitor%d.txt",id);

	FILE *leptr = fopen(nome_arquivo, "a");
	fprintf(leptr,"%d\n",conteudo);

	debug("\t%d leu conteudo=%d\n",id,conteudo);
	fclose(leptr);
}


void entra_fila(int tipo, int id, int senha){
	//gera_log(tipo,id,ESPERANDO);

	pthread_mutex_lock(&mutexfila);
	while(senha_atual!=senha){
	//	gera_log(tipo,id,ESPERANDO);

		debug("SENHA LOCAL: %d SENHA ATUAL: %d\n",senha,senha_atual);
		pthread_cond_wait(&condfila,&mutexfila);
	}
	debug("\tSENHA %d conseguiu entrar\n",senha);
	pthread_mutex_unlock(&mutexfila);
}

void sai_fila(){
	pthread_mutex_lock(&mutexfila);
	senha_atual++;
	debug("senha atual saindo: %d\n",senha_atual);
	pthread_cond_broadcast(&condfila);
	pthread_mutex_unlock(&mutexfila);
}

void entra_l(int id, int senha){
	entra_fila(LEITOR, id, senha);

	pthread_mutex_lock(&mutex);
	
	while(escritores_ativos){
	//	gera_log(LEITOR,id,ESPERANDO);
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
	
//	gera_log(ESCRITOR,id,ESPERANDO);

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
		
		gera_log_leitura(id,conteudo);


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

int main(int agrc, char *argv[]){
	int num_threads_l, num_threads_e;
	
	pthread_t *tidl, *tide;
	int *idl, *ide;
	
	printf("Numero de threads leitoras: ");
	scanf("%d", &num_threads_l);

	printf("Numero de threads escritoras: ");
	scanf("%d", &num_threads_e);

	printf("Numero de leituras: ");
	scanf("%d", &num_l);

	printf("Numero de escritas: ");
	scanf("%d", &num_e);

	tidl = malloc(num_threads_l*sizeof(pthread_t));
	if(tidl == NULL){
		printf("Erro na alocação de memória de tidl");
	}

	tide = malloc(num_threads_e*sizeof(pthread_t));
	if(tide == NULL){
		printf("Erro na alocação de memória de tide");
	}

	idl = malloc(num_threads_l*sizeof(int));
	if(idl == NULL){
		printf("Erro na alocação de memória de idl");
	}

	ide = malloc(num_threads_e*sizeof(int));
	if(ide == NULL){
		printf("Erro na alocação de memória de ide");
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
			printf("ERRO\n");exit(-1);
		}
	}
	
	for(int t=0;t<num_threads_e;t++){
		ide[t]=t;
		if(pthread_create(&tide[t], NULL, escrever, (void*) &ide[t])){
			printf("ERRO\n");exit(-1);
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

	pthread_exit(NULL);
	
}
