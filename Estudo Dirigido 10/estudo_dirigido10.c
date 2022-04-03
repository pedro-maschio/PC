/*
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila
int cnt_cartas = 0; // quantidade atual de cartas

pthread_cond_t cond_pombo = PTHREAD_COND_INITIALIZER; // variável de condição do pombo
pthread_cond_t cond_entrega = PTHREAD_COND_INITIALIZER;  // variável de condição para liberar a entrega (voo) das cartas quando a mochila estiver cheia
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

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
        pthread_mutex_lock(&mutex);
        
            while(cnt_cartas < CARTAS) {
                pthread_cond_wait(&cond_pombo, &mutex);
            }
            //Leva as cartas para B e volta para A 
            printf("O pombo partiu de A para B\n");
            sleep(1);
            printf("O pombo entregou as cartas em B\n");

            cnt_cartas = 0; // após entregar as cartas, o pombo fica com 0 cartas

            printf("O pombo voltou para A e está pronto pra trabalhar!\n");
            sleep(1);

        pthread_mutex_unlock(&mutex);
    }
}

void * f_usuario(void *arg){
    int id = *((int *)arg);
    while(1){
	    //Escreve uma carta
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        //Caso a mochila fique cheia, acorda o ṕombo

        pthread_mutex_lock(&mutex);
            
            // Se a mochila estiver cheia, espera
            while(cnt_cartas == CARTAS) {
                pthread_cond_wait(&cond_entrega, &mutex);
            }

            // Escreve uma carta
            cnt_cartas++;
            printf("O usuário %d escreveu uma carta, a quantidade de cartas escritas eh: %d\n", id, cnt_cartas);
            sleep(1); // tempo para escrever a carta

            if(cnt_cartas == CARTAS) {
                // Usamos signal em vez de broadcast pois só há um pombo
                pthread_cond_signal(&cond_pombo);
            }
            
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}
