/*
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de andares
#define C 5 // capacidade de pessoas no elevador

int direcao = 1; // direcao do elevador, 1 subindo 0 descendo
int andar = 0; // andar atual do elevador
int total_no_elevador = 0; // quantidade de pessoas no elevador no momento
int pessoas_andar[N]; // indica a quantidade de pessoas no andar i
int solicitado = 1; // flag de controle para exibir as mensagens
int emDescida = 0; // flag para evitar que o elevador volte a subir após começar a descer

pthread_mutex_t mutex; // mutex para proteger a posição do elevador
pthread_mutex_t leituraVetor;
pthread_cond_t elevador_cond = PTHREAD_COND_INITIALIZER;


void * f_andar(void *arg);
void * f_elevador(void *arg);
int ha_pessoas_nos_andares();
int pessoas_acima(int);

void elevador_parado(int);
void elevador_solicitado();

int main(int argc, char **argv){
    int i;

    for(int i = 0; i < N; i++) {
        pessoas_andar[i] = 0; // inicialmente há 0 pessoas em cada andar
    }

    pthread_t andar[N];
    int *id;
    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(andar[i]),NULL,f_andar,  (void *) (id));
    }

    pthread_t elevador;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(elevador),NULL,f_elevador, (void*) (id));

    pthread_join(elevador,NULL);
}

void * f_andar(void *arg){
    int id = *((int *)arg);
    while(1){
        sleep(1); // as pessoas demoram a chegar nos andares
        pthread_mutex_lock(&mutex); // região crítica
            int qtd_pessoas = rand() % C ; // há no máximo a capacidade do elevador em pessoas em cada andar - 1
            pessoas_andar[id] = qtd_pessoas;
        pthread_mutex_unlock(&mutex);

        if(qtd_pessoas > 0)
            pthread_cond_signal(&elevador_cond); // sinaliza ao elevador que há pessoas em algum andar
    }
}

void * f_elevador(void *arg){
    int id = *((int *)arg);
    
    while(1){
        pthread_mutex_lock(&mutex);

            while(total_no_elevador == 0 && !ha_pessoas_nos_andares()) {
                elevador_parado(andar); // exibe a mensagem e para a thread
            }
            if(solicitado == 1) {
                elevador_solicitado();
            }

            if(pessoas_acima(andar) && !emDescida) { // se há pessoas acima da posição do elevador
                direcao = 1;
            } else {
                direcao = 0;
            }
            
            // Lógica principal para definir se o elevador vai pegar as pessoas
            if(pessoas_andar[andar] > 0) {
                if(pessoas_andar[andar] + total_no_elevador >= C) { // cabem no máximo C pessoas no elevador;
                    pessoas_andar[andar] = C - total_no_elevador;
                    total_no_elevador = C;
                    direcao = 0; // já encheu, hora de descer
                    emDescida = 1; 
                } else {
                    total_no_elevador += pessoas_andar[andar];
                    pessoas_andar[andar] = 0; // entraram todas as pessoas, o andar ficou vazio
                }
            }

            printf("O elevador está no andar %d, levando %d pessoas\n", andar, total_no_elevador);

            if(direcao == 1) {
                andar += 1;
            } else {
                andar -= 1;
            }

            if(andar == 0) {
                printf("O elevador chegou ao térreo, atualmente está com zero pessoas\n");
                total_no_elevador = 0;
                solicitado = 1;
                emDescida = 0; 
                direcao = 1; // ao chegar no térreo, ele só pode subir
            }
            sleep(1); // o elevador não é instantâneo
        pthread_mutex_unlock(&mutex);
    }
}

int ha_pessoas_nos_andares() {
    pthread_mutex_lock(&leituraVetor);
        for(int i = 0; i < N; i++) {
            if(pessoas_andar[i] > 0) {
                pthread_mutex_unlock(&leituraVetor);
                return 1;
            }
        }
    pthread_mutex_unlock(&leituraVetor);
    
    return 0;
}

int pessoas_acima(int andar) {
    pthread_mutex_lock(&leituraVetor);
        for(int i = andar; i < N; i++) {
            if(pessoas_andar[i] > 0) {
                pthread_mutex_unlock(&leituraVetor);
                return 1;
            }
        }
    pthread_mutex_unlock(&leituraVetor);
    return 0;
}

void elevador_parado(int andar) {
    printf("Não há pessoas solicitando o elevador, o elevador vazio está parado no andar %d\n", andar);
    pthread_cond_wait(&elevador_cond, &mutex);
}

void elevador_solicitado() {
    printf("Elevador solicitado, o elevador está ");
    if(direcao == 0)
        printf("descendo\n");
    else 
        printf("subindo\n");
    solicitado = 0;
}




