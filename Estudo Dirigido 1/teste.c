#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define N 4
#define X 100000

int contador = 0;
pthread_mutex_t lock_contador = PTHREAD_MUTEX_INITIALIZER;

void * incrementar(void *arg) {
    int id = *((int *) arg);
    printf("Criou thread com id = %d\n", id);
    for(int i = 0; i < X; i++) {
        // pegar o lock
        pthread_mutex_lock(&lock_contador);
        contador++;
        pthread_mutex_unlock(&lock_contador);
    }
    pthread_exit(0);
}


int main() {
    pthread_t a[N];

    int *id;

    for(int i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, incrementar, (void *)(id));
    }

    for(int i = 0; i < N; i++) {
        pthread_join(a[i], NULL);
    }

    printf("Valor final do contador = %d\n", contador);
    printf("FIM\n\n");


    return 0;
}