#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10
#define INCREMENTA 10000

int contador = 0;

void * pthread_func(void *arg) {
    int id = *((int *)arg);
    printf("Criou um pthread com id = %d\n", id);

    for(int i = 0; i < INCREMENTA; i++)
        contador++;
    pthread_exit(0);
}

int main() {
    pthread_t a[N];
    printf("Meu id: %lu\n", pthread_self());

    int i;
    int *id;    

    for(i = 0; i < N; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *)(id));
    }

    for(i = 0; i < N; i++) {
        pthread_join(a[i], NULL);
    }
    printf("Valor final do contador: %d\n", contador);
    printf("Terminando\n");


    return 0;
}