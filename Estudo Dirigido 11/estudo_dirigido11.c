/*
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;  
int size, valorInicial; // tamanho e valor inicial que a matriz vai receber


int matriz[MAXSIZE][MAXSIZE];
int *sum; // vetor para armazenar as somas parciais por linha da matriz

void *Worker(void *);

/*
  Este programa receber como argumentos o tamanho da matriz e o valor inicial
  de seus elementos.
*/

int main(int argc, char *argv[]) {
  int i, j;
  
  size = atoi(argv[1]);
  valorInicial = atoi(argv[2]);
  
  if(size > MAXSIZE){
	  printf("Tamanho muito grande!\n");
    return 0;	
  }

  pthread_t workerid[size];
  sum = malloc(sizeof(int)*size); // alocando o vetor sum
  

  pthread_barrier_init(&barrier, NULL, size);

  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      matriz[i][j] = valorInicial;

  int * id;
  for (i = 0; i < size; i++){
     id = (int *) malloc(sizeof(int));
     *id = i;
     pthread_create(&workerid[i], NULL, Worker, (void *) (id));

  }

  for (i = 0; i < size; i++){
       if(pthread_join(workerid[i], NULL)){
          printf("\n ERROR joining thread");
          exit(1);
       }
  }
  printf("Bye!\n");
}


void *Worker(void *arg) {
  int myid = *(int *)(arg);
  int j, k;

  int self = pthread_self();
 
  printf("worker %d (pthread id %d) has started\n", myid, self);


  // Soma todos os elementos dessa linha
  for(j = 0; j < size; j++){
    sum[myid] += matriz[myid][k];
  }

  pthread_barrier_wait(&barrier);

  if (myid == 0) {
    // A thread zero soma os valores parciais
    int somaTotal = 0;
    for(int i = 0; i < size; i++) {
      somaTotal += sum[i];
    }
    printf("\n ");
    for(j = 0; j < size; j++){
      for(k = 0; k < size; k++){
        printf("%d ",matriz[j][k]);   
      }
      printf("\n ");
      }
    
    printf("A soma dos elementos dessa matriz eh: %d\n", somaTotal);
  }


}
