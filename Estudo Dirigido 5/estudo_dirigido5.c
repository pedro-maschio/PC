/**
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

void *canibal(void *meuid);
void *cozinheiro(int m);

pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER; // variável de condição do cozinheiro
pthread_cond_t canibal_cond = PTHREAD_COND_INITIALIZER; // variável de condição do canibal
pthread_mutex_t lock_porcoes = PTHREAD_MUTEX_INITIALIZER; // lock da variável de número de porções

int num_porções = 0; // número atual de porções, inicialmente zerado

void main(argc, argv) int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if (argc != 3) {
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }

  
  n = atoi(argv[1]); // número de canibais
  m = atoi(argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if (n > MAXCANIBAIS) {
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }

  for (i = 0; i < n; i++) {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *)(id));

    if (erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void *canibal(void *pi) {

  while (1) {
    /*
      O canibal pega o lock. Se as porções acabarem, avisa o cozinheiro para produzir mais
      e depois vai dormir.
    */ 
    pthread_mutex_lock(&lock_porcoes);
      while(num_porções == 0) {
        printf("As porções acabaram! Acordando cozinheiro!\n");

        // Notifica o cozinheiro para produzir mais
        pthread_cond_signal(&cozinheiro_cond);
        
        // Dorme enquanto as porções são preparadas
        pthread_cond_wait(&canibal_cond, &lock_porcoes);
      }
      
      num_porções--; // consome uma porção
      printf("%d: vou comer a porcao que peguei\n", *((int *)pi));
      sleep(5);

    pthread_mutex_unlock(&lock_porcoes);
  }
}

void *cozinheiro(int m) {

  while (1) {
    /* 
      O cozinheiro pega o lock, enquanto houver porções disponíveis, dorme. Quando as porções
      acabam, produz mais m porções e avisa os canibais.
    */
    pthread_mutex_lock(&lock_porcoes);
      while(num_porções > 0) {
        printf("Ainda há %d porções disponíveis, cozinheiro indo dormir.\n", num_porções);
        pthread_cond_wait(&cozinheiro_cond, &lock_porcoes);
      } 

      num_porções += m; // o cozinheiro produz m porções
      printf("cozinheiro: vou cozinhar\n");
      sleep(5);
      
      // acordar os canibais
      pthread_cond_signal(&canibal_cond);

    pthread_mutex_unlock(&lock_porcoes);
  }
}
