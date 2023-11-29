/*
Benjamin Harris and Jeremiah Harris
Project 2 Part 2
CS 425
11/27/23

Producer Consumer problem using Spin locks
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

struct v{
    int tid;
    int buffer_size;
    int upper_limit;
};


//Function declarations
void *producer(void *);
void *consumer(void *);

//Shared Variables
pthread_spinlock_t lock;
sem_t full, empty;
int consumedCount = 0;
int producedCount = 0;
int *buffer;
int in = 0;
int out = 0;

int main(int argc, char *argv[]){

    printf("\nUsing Spinlock\n");

    int bufferSize = atoi(argv[1]);
    buffer =  (int*) malloc(sizeof(int) * bufferSize);

    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t tid_producer[num_producers], tid_consumer[num_consumers];
    pthread_attr_t attr_producer[num_producers], attr_consumer[num_consumers];
    
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, bufferSize);
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

    
    int i;
    for(i = 0; i < num_producers; i++){
        struct v *data = (struct v *) malloc(sizeof(struct v));

        data->tid = i;
        data->buffer_size = bufferSize;
        data->upper_limit = atoi(argv[4]);

        pthread_attr_init(&attr_producer[i]);
        pthread_create(&tid_producer[i], &attr_producer[i], producer, data);
    }

    for(i = 0; i < num_consumers; i++){
        struct v *data = (struct v *) malloc(sizeof(struct v));

        data->tid = i;
        data->buffer_size = bufferSize;
        data->upper_limit = atoi(argv[4]);

        pthread_attr_init(&attr_consumer[i]);
        pthread_create(&tid_consumer[i], &attr_consumer[i], consumer, data);
    }

    for(i = 0; i < num_producers; i++){
        pthread_join(tid_producer[i], NULL);
    }

    for(i = 0; i < num_consumers; i++){
        pthread_join(tid_consumer[i], NULL);
    } 

    return 0;
}

//Producer Function
void *producer(void *param){
    struct v *data;
    data = (struct v*) param;

    while(producedCount<= data->upper_limit){
        sem_wait(&empty);
        pthread_spin_lock(&lock);

        int next_produced = producedCount;
        producedCount++;
        buffer[in] = next_produced;
        in = (in + 1) % data->buffer_size; 

        pthread_spin_unlock(&lock);
        sem_post(&full);
    }

    pthread_exit(NULL);
}

//Consumer Function
void *consumer(void *param){
    struct v *data;
    data = (struct v*) param;


    while(consumedCount < data->upper_limit){
        sem_wait(&full);
        pthread_spin_lock(&lock);

        int item = buffer[out];
        printf("%d %d\n", item, data->tid);
        out = (out + 1) % data->buffer_size;

        consumedCount++;

        pthread_spin_unlock(&lock);
        sem_post(&empty);

    }

    pthread_exit(NULL);
}