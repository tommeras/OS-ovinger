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
    sem -> value = initVal;
    sem -> cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    sem -> mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    return sem; 
};


int sem_del(SEM *sem){
    int ret = 0;
    if(pthread_mutex_destroy(&sem->mutex)!=0){
        ret=-1;
    };
    if(pthread_cond_destroy(&sem->cond)!=0){
        ret=-1;
    };
    free(sem);
    return ret;


};

void P(SEM *sem){
    pthread_mutex_lock(&(sem->mutex));

    while(sem -> value <= 0){
        pthread_cond_wait(&(sem->cond),&(sem->mutex));
    }
    sem->value--;
    pthread_mutex_unlock(&(sem -> mutex));

    
    

};

void V(SEM *sem){
    pthread_mutex_lock(&(sem->mutex));
    sem->value++;
    pthread_cond_signal(&(sem->cond));
    pthread_mutex_unlock(&(sem -> mutex));
};

