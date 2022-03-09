/**
 * Aluno: Pedro de Torres Maschio
 * Matrícula: 190018763
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A
#define G 2 // número de gorilas de A para B

pthread_mutex_t lock_ab = PTHREAD_MUTEX_INITIALIZER; // lock do contador de macacos de A para B
pthread_mutex_t lock_ba = PTHREAD_MUTEX_INITIALIZER; // lock do contador de macacos de B para A
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER; // lock da corda (NA REAL É ESSE QUE IMPEDE SENTIDO DISTINTO, COMENTEI ERRADO NO ENVIO )
pthread_mutex_t lock_macacos = PTHREAD_MUTEX_INITIALIZER; // lock para impedir macacos transitando em sentidos distintos

int num_macacos_ab = 0;
int num_macacos_ba = 0;


void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
      //Procedimentos para acessar a corda

      /*
        Se for o primeiro macaco de A para B, pega o lock da corda, de tal forma que os próximos não precisam
        fazer esse passo. Independentemente da ordem, sempre pega o lock de macacos, isso impede que haja
        macacos em dois sentidos atravessando simultaneamente a corda.
      */
      pthread_mutex_lock(&lock_macacos);
        pthread_mutex_lock(&lock_ab); // para evitar a condição corrida no acesso à variável lock_ab
          num_macacos_ab++;
          if(num_macacos_ab == 1) 
            pthread_mutex_lock(&lock_corda);
        pthread_mutex_unlock(&lock_ab);
      pthread_mutex_unlock(&lock_macacos);
    

        printf("Macaco %d passado de A para B \n",i);
        sleep(1);
      
      //Procedimentos para quando sair da corda
      /*  
        Ao sair da corda, decrementa o contador de macacos de A para B, e se for o último, libera o lock da corda.
      */
      pthread_mutex_lock(&lock_ab);
        num_macacos_ab--;
        if(num_macacos_ab == 0)
          pthread_mutex_unlock(&lock_corda);
      pthread_mutex_unlock(&lock_ab);
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
      //Procedimentos para acessar a corda
      /*
        Se for o primeiro macaco de B para A, pega o lock da corda, de tal forma que os próximos não precisam
        fazer esse passo. Independentemente da ordem, sempre pega o lock de macacos, isso impede que haja
        macacos em dois sentidos atravessando simultaneamente a corda.
      */
      pthread_mutex_lock(&lock_macacos);
        pthread_mutex_lock(&lock_ba); // para evitar a condição corrida no acesso à variável lock_ba
          num_macacos_ba++;
          if(num_macacos_ba == 1) 
            pthread_mutex_lock(&lock_corda);
        pthread_mutex_unlock(&lock_ba);
      pthread_mutex_unlock(&lock_macacos);

        printf("Macaco %d passado de B para A \n",i);
        sleep(1);

      //Procedimentos para quando sair da corda
      /*
        Ao sair da corda, decrementa o contador de macacos de B para A, e se for o último, libera o lock da corda.
      */
      pthread_mutex_lock(&lock_ba);
        num_macacos_ba--;
        if(num_macacos_ba == 0)
          pthread_mutex_unlock(&lock_corda);
      pthread_mutex_unlock(&lock_ba);

    }
    pthread_exit(0);
}

void * gorila(void * a){
  int i = *((int *) a);   

  while(1){
    //Procedimentos para acessar a corda
    /*  
      O gorila pega o lock de macacos, para que não tenha nenhum outro macaco transitando ao mesmo tempo.
    */
    pthread_mutex_lock(&lock_macacos);
      pthread_mutex_lock(&lock_corda);
    pthread_mutex_unlock(&lock_macacos);

    printf("Gorila %d passado de A para B \n", i);
    sleep(5);

    //Procedimentos para quando sair da corda
    /*
      Ao sair da corda, libera o lock da corda
    */
    pthread_mutex_unlock(&lock_corda);

  }
  pthread_exit(0);
}

int main(int argc, char * argv[]) {
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    
    // Adicionei 3 gorilas para verificar como a solução ia se comportar.
    pthread_t g[G];
    for(i = 0; i < G; i++) {
      id = (int *) malloc(sizeof(int));
      *id = i;
      pthread_create(&g[i], NULL, &gorila, (void *)(id));
    }

  
    pthread_join(macacos[0], NULL);
    return 0;
}
