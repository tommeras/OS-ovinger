#include "sem.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int value;
int pshared;

typedef struct SEM{
    int value;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}SEM;

SEM *sem_init(int initVal) {
    SEM *sem = malloc(sizeof(struct SEM));
    //SEM *new;
    sem -> value = initVal;
    sem -> cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    sem -> mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    //(*new).value = 1;

    return sem; 
};


int sem_del(SEM *sem){
    free(sem);

    if(sem->value){
        return 0;
    }
    else{
        return -1;
    }
};

void P(SEM *sem){
    if(sem -> value > 0){
            sem->value--;
    }
    else{
        pthread_mutex_lock(&(sem->mutex));
        pthread_cond_wait(&(sem->cond),&(sem->mutex));
        pthread_mutex_unlock(&(sem -> mutex));
    }

    /* while(sem->value == 0){
        int i = pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->value = sem->value-1; */
    
    

};

void V(SEM *sem){
    pthread_mutex_lock(&(sem->mutex));
    if(sem -> value <= 0){
        pthread_cond_signal(&(sem->cond));
    }
    sem->value++;
    pthread_mutex_unlock(&(sem -> mutex));


};


void main(){
    SEM *sem = sem_init(2);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    V(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    V(sem);
    printf("%d\n", sem->value);


   
}