#include "sem.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int value;
int pshared;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct SEM{
    int value;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}SEM;

SEM *sem_init(int initVal) {
    SEM *sem = malloc(sizeof(struct SEM));
    //SEM *new;
    sem -> value = initVal;
    //(*new).value = 1;

    return sem; 
};


int sem_del(SEM *sem){

};

void P(SEM *sem){
    if(sem->value==0){
        int i = pthread_cond_wait(&sem->cond, &sem->mutex);
        if(i=0){
            printf("i=0\n");
        }
        else{
            printf("i=/=0\n");
        }

    }
    else{
        sem->value = sem->value-1;
    }
    
    

};

void V(SEM *sem){

    sem->value = sem->value+1;


};


void main(){
    SEM *sem = sem_init(2);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);
    P(sem);
    printf("%d\n", sem->value);


}