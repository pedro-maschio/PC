/**
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3 // tamanho do buffer
#define NP 1 // número de produtores
#define NC 1 // número de consumidores


void *producer(void *meuid);
void *consumer(void *meuid);

pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER; // mutex para proteger o contador

sem_t empty; // conta os lugares vazios no buffer
sem_t full; // conta os lugares cheios no buffer

int contador = 0;

int main() {
  int erro;
  int i, n, m;
  int *id;

  sem_init(&empty, 0, N); // inicialmente o N estão vazios
  sem_init(&full, 0, 0); // inicialmente há 0 cheios


  pthread_t produtores[NP];
  for (i = 0; i < NP; i++) {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&produtores[i], NULL, producer, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t consumidores[NC];
  for (i = 0; i < NC; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&consumidores[i], NULL, consumer, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_join(produtores[0], NULL);
}

void *producer(void *a) {
  int i = *((int *)a);
  while (1)
  {
    sleep(rand() % 2);

    /*
        Foi implementada a solução mostrada em aula. O produtor tira um elemento
        de empty e coloca em full.
    */
    

    sem_wait(&empty);
    
    pthread_mutex_lock(&mutex_buffer);
        contador++; // aumenta o número de itens no buffer
        printf("Produtor %d produziu um item, elementos no buffer = %d\n", i, contador);
    pthread_mutex_unlock(&mutex_buffer);

    sem_post(&full);

    sleep(1);

   
  }
  pthread_exit(0);
}

void *consumer(void *a) {
  int i = *((int *)a);
  while (1)
  {
    sleep(rand() % 2);

    /*
        Foi implementada a solução mostrada em aula. O produtor coloca um elemento
        em empty e tira de full.
    */
    
    sem_wait(&full);

    pthread_mutex_lock(&mutex_buffer);
        contador--;
        printf("Consumidor %d produziu um item, número de elementos no buffer = %d\n", i, contador);
    pthread_mutex_unlock(&mutex_buffer);

    sem_post(&empty);

    sleep(1);

    
  }
  pthread_exit(0);
}

