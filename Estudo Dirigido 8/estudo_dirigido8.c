/*
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id)	(id+1)%N


sem_t s[N];

int estado_filosofo[N]; // 0 = com fome, 1 = pensando, 2 = comendo
#define COMFOME 0 
#define PENSANDO 1
#define COMENDO 2
pthread_mutex_t lock_estado; // mutex para proteger as mudanças de estado


void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);
void tenta_comer(int n);

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
	     id = (int *) malloc(sizeof(int));
	     *id = i;
	     pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
        printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {
	pthread_mutex_lock(&lock_estado);
		estado_filosofo[n] = COMFOME;

		tenta_comer(n);

		sem_wait(&s[n]); // se não consegue comer, espera
	pthread_mutex_unlock(&lock_estado);
}

void tenta_comer(int n) {
	/*
		Condições necessárias para o filósofo poder comer: estar
		com fome e os vizinhos não podem estar comendo. 
	*/
	if(estado_filosofo[n] == COMFOME && 
		estado_filosofo[ESQ(n)] != COMENDO && 
		estado_filosofo[DIR(n)] != COMENDO) {

		estado_filosofo[n] = COMENDO;
		

		sem_post(&s[n]); // já comeu, devolve o semáforo
	}
}

void devolve_talher (int n) {
	pthread_mutex_lock(&lock_estado); // pega o lock para realizar a mudança de estado
		estado_filosofo[n] = PENSANDO;
		printf("Filosofo %d devolveu os talheres\n", n);

		// Ao devolver os talhers, os vizinhos vão tentar comer também
        tenta_comer(ESQ(n));
        tenta_comer(DIR(n));

	pthread_mutex_unlock(&lock_estado);
}

