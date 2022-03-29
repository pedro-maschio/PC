#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila
int cnt_cartas = 0; // quantidade atual de cartas

sem_t sem_cartas; // semáforo que representa a qtd de cartas
sem_t sem_pombo; // mutex do pombo 
sem_t sem_entrega; // semáforo para liberar a entrega (voo) das cartas

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

    // Semáforos iniciados
    sem_init(&sem_cartas, 0, CARTAS);
    sem_init(&sem_pombo, 0, 1);
    // iniciando com 0 pois inicialmente o pombo não pode entregar
    sem_init(&sem_entrega, 0, 0);


    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }

    pthread_t pombo;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

    pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        

        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        sem_wait(&sem_entrega); // com a permissão recebida, o pombo pode voar
        sem_wait(&sem_pombo);
            //Leva as cartas para B e volta para A 
            printf("O pombo partiu de A para B\n");
            sleep(1);
            printf("O pombo entregou as cartas em B\n");

            // Acordar os usuários para que eles escrevam novas cartas
            for(int i = 0; i < CARTAS; i++) {
                sem_post(&sem_cartas);
                cnt_cartas--;
            }

            printf("O pombo voltou para A e está pronto pra trabalhar!\n");
            sleep(1);

        sem_post(&sem_pombo);
    }
}

void * f_usuario(void *arg){
    int id = *((int *)arg);
    while(1){
	    //Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo

        sem_wait(&sem_pombo);
            sem_wait(&sem_cartas); // pega uma carta

            cnt_cartas++;
            printf("O usuário %d escreveu uma carta, a quantidade de cartas escritas eh: %d\n", id, cnt_cartas);
            //sleep(1); // tempo para escrever a carta

            if(cnt_cartas == CARTAS) {
                sem_post(&sem_entrega); // dá a permissão pro pombo entregar as cartas
            }
            
        sem_post(&sem_pombo);
        sleep(1);
    }
}
