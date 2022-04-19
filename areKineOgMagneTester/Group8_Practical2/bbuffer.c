#include "bbuffer.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "sem.c"



typedef struct BNDBUF{
    int *buf;
    unsigned int buf_size;
    volatile unsigned int write_index;
    volatile unsigned int read_index;

    SEM *open_slots;
    SEM *full_slots;
    SEM *write_mutex;
    SEM *read_mutex;
}BNDBUF;



BNDBUF *bb_init(unsigned int size){
    BNDBUF *bbuf=calloc(1, sizeof(struct BNDBUF));
        bbuf -> buf = malloc(sizeof(int)*size);
        bbuf -> buf_size = size;

        bbuf -> open_slots = sem_init(size);
        bbuf -> full_slots = sem_init(0);

        bbuf -> read_mutex = sem_init(1);
        bbuf -> write_mutex = sem_init(1);
    return bbuf;
};


void bb_del(BNDBUF *bb){

    free(bb);
};
    

int  bb_get(BNDBUF *bb){
    int fd;
    P(bb->full_slots);
    P(bb->read_mutex);
    fd = bb->buf[bb->read_index];
    bb->read_index = (bb->read_index+1) % bb->buf_size;
    V(bb->read_mutex);
    V(bb->open_slots);
    return fd;
};


void bb_add(BNDBUF *bb, int fd){
    P(bb->open_slots);

    P(bb->write_mutex);

    bb->buf[bb->write_index] = fd;

    bb->write_index = (bb->write_index+1) % bb -> buf_size;

    V(bb -> write_mutex);

    V(bb -> full_slots);
};


