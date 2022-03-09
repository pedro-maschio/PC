/**
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N 3 // tamanho do buffer
#define NP 1 // número de produtores
#define NC 1 // número de consumidores


void *producer(void *meuid);
void *consumer(void *meuid);

pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER; // mutex para proteger o contador
pthread_cond_t produtor_cond = PTHREAD_COND_INITIALIZER; // condicional do produtor
pthread_cond_t consumidor_cond = PTHREAD_COND_INITIALIZER; // condicional do consumidor

int contador = 0; // número de elementos no buffer no momento

int main() {
  int erro;
  int i, n, m;
  int *id;


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
      Foi utilizado um lock para proteger o contador do buffer. Quando o contador atinge o limite do buffer, a
      variável de condição é utilizada para adormecer o produtor.
    */
    pthread_mutex_lock(&mutex_buffer);

      while (contador == N) { // se o buffer está cheio, então dorme
        printf("Buffer cheio, produtor %d dormindo! zzzZZ!\n", i);
        pthread_cond_wait(&produtor_cond, &mutex_buffer);
      }

      contador++; // aumenta o número de itens no buffer
      printf("Produtor %d produziu um item, elementos no buffer = %d\n", i, contador);
  
      sleep(1);

      if (contador == 1) { // se o contador é igual a 1, então anteriormente o buffer estava vazio
        printf("Acordando o consumidor, número de elementos no buffer = %d\n", contador);
        pthread_cond_signal(&consumidor_cond); // sinaliza thread consumidora que o buffer tem espaço
      }

    pthread_mutex_unlock(&mutex_buffer);
  }
  pthread_exit(0);
}

void *consumer(void *a) {
  int i = *((int *)a);
  while (1)
  {
    sleep(rand() % 2);

    /*
      Foi utilizado um lock para proteger o contador do buffer. Quando o contador tem tamanho zero, então o consumidor
      adormece para esperar novos itens serem produzidos.
    */
    pthread_mutex_lock(&mutex_buffer);

      while(contador == 0) { // enquanto não tiver itens no buffer para consumir, dorme
        printf("Buffer vazio, consumidor %d dormindo! zzzZZ!\n", i);
        pthread_cond_wait(&consumidor_cond, &mutex_buffer);
      }

      contador--;
      printf("Consumidor %d produziu um item, número de elementos no buffer = %d\n", i, contador);

      sleep(1);

      if(contador == N - 1) {
        printf("Acordando o produtor, número de elementos no buffer = %d\n", contador);
        pthread_cond_signal(&produtor_cond); // sinaliza a thread produtora que o buffer agora tem espaço
      }

    pthread_mutex_unlock(&mutex_buffer);
  }
  pthread_exit(0);
}

