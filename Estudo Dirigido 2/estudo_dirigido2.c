/**
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define NE 3 // número escritores
#define NL 10 // número de leitores


pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER; // lock do banco de dados 
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER; // lock do contador de leitores 
pthread_mutex_t lock_starvation = PTHREAD_MUTEX_INITIALIZER; // lock para resolver o starvation dos escritores

int num_leitores = 0;


void *reader(void *arg);
void *writer(void *arg);
void read_data_base(int i);
void use_data_read(int i);
void think_up_data(int i);
void write_data_base(int i);

int main() {
    pthread_t r[NL], w[NE];
    int i;
    int *id;

    /* criando leitores */
    for(i = 0; i < NL; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&r[i], NULL, reader, (void *)(id));
    }

    /* criando escritores */
    for(i = 0; i < NE; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&w[i], NULL, writer, (void *)(id));
    }

    pthread_join(r[0], NULL);

    return 0;
}

void *reader(void *arg) {
    int i = *((int *) arg);
    
    while(1) { // repete para sempre

        /*  
            O leitor pega o lock de starvation, e se for o primeiro leitor
            também pega o lock do bd. Caso haja um escritor com o lock de starvation,
            o leitor fica bloqueado esperando.
        */
        pthread_mutex_lock(&lock_starvation);
            pthread_mutex_lock(&lock_nl); // para evitar a condição corrida no acesso à variável num_leitores
                num_leitores++;
                if(num_leitores == 1)
                    pthread_mutex_lock(&lock_bd);
            pthread_mutex_unlock(&lock_nl);
        pthread_mutex_unlock(&lock_starvation);

        read_data_base(i); // acesso ao banco de dados

        /*
            Quando o último leitor terminar de ler, ele libera o lock do bd para
            permitir que os escritores (ou novos leitores) consigam acessar o bd novamente .
        */
        pthread_mutex_lock(&lock_nl);
            num_leitores--;
            if(num_leitores == 0)
                pthread_mutex_unlock(&lock_bd);
        pthread_mutex_unlock(&lock_nl);
        
        use_data_read(i); // região não crítica
    }
    pthread_exit(0);
}

void *writer(void *arg) {
    int i = *((int *) arg);

    while(1) { // repete para sempre
        think_up_data(i); // região não crítica

        /*
            Para evitar o starvation, o escritor pega o lock de starvation e isso impede que 
            os leitores tentem fazer o mesmo.
        */
        pthread_mutex_lock(&lock_starvation);
            pthread_mutex_lock(&lock_bd);
                write_data_base(i); // atualiza os dados (região crítica)
            pthread_mutex_unlock(&lock_bd);
        pthread_mutex_unlock(&lock_starvation);
    }
    pthread_exit(0);
}

void read_data_base(int i) {
    printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5);
}

void use_data_read(int i) {
    printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5);
}

void think_up_data(int i) {
    printf("Escritor %d está pensando no que escrever!\n", i);
    sleep(rand() % 5);
}

void write_data_base(int i) {
    printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5 + 15);  
}