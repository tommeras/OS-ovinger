#include "sem.h"

int value;
int pshared;
int ret;

struct SEM{
    int value;
    int pshared;
    int ret;
}

Tem_init(int initVal) {
    SEM new;
    new.value = initVal;
    new.pshared = 0;

    return new; 
};


int sem_del(SEM *sem){

};

void P(SEM *sem){
    sem->value = sem->value-1;
    

};

void V(SEM *sem){

    sem->value = sem->value+1;


};