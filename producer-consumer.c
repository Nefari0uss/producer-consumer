#include <stdlib.h>
#include <stdio.h>
#include <time.h> // time for seeding
#include <pthread.h> // include pthread functions and structures
#include <semaphore.h> // include semaphores
#include <unistd.h> // include sleep

/** Producer - Consumer Simulation
 *  @author Nefari0uss
 *  
 *  This simulation was to help learn about threading in C using
 *  producers and consumers. Producers create and place an int in
 *  a buffer. Consumers will attempt to remove ints from the
 *  buffer. 
 *
 *  To run: ./pc.out x y z where x, y, z is some integer specifying the
 *  sleep time, number of producers, number of consumers.
 **/

/** The Buffer **/
typedef int buffer_item;
#define BUFFER_SIZE 7
buffer_item buffer[BUFFER_SIZE];
int insertIndex;
int removeIndex;

/** Semaphores & Mutexes **/
sem_t full;
sem_t empty;
pthread_mutex_t mutex;

/** Seed **/
unsigned int seed;

/** Other Variables **/
int SLEEP_TIME;


/** Print Buffer to view contents. **/
void printBuffer() {
    printf("\t\tPrinting Buffer: [");
    int i;
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        printf("%d ", buffer[i]); 
    }
    printf("]\n");

}

/** Buffer Methods **/
int insert_item(buffer_item item) {

    if (buffer[insertIndex] != '\0') {
        //printf("Could not insert into buffer. An item already exists!\n");
        return -1;

    } else {
        /* Insert an item into buffer */
        buffer[insertIndex] = item;

        /* Adjust insertIndex position. Increment and modulo for circular buffer. */
        insertIndex++;
        insertIndex = insertIndex % BUFFER_SIZE;

        printf("---> Produced: %d\n", item);
        printBuffer();
        return 0;

    }

}

int remove_item(buffer_item item) {

    if (buffer[removeIndex] == '\0') {
        //printf("Could not remove from buffer. The item does not exist!\n");
        return -1;

    } else {
        /* Remove an object from buffer and place it in item*/
        buffer_item removedItem = buffer[removeIndex];
        buffer[removeIndex] = '\0';

        /* Adjust removeIndex position. Increment and modulo for circular buffer. */
        removeIndex++;
        removeIndex = removeIndex % BUFFER_SIZE;

        printf("<--- Consumed: %d\n", removedItem);
        printBuffer();
        return 0;

    }

}

/** End Buffer Methods **/

/** Producer Entry **/
void *producer(void *param) {
    buffer_item item;
    while (1) {
        printf("in producer.\n");
        /* Sleep for a random period of time between 1 and SLEEP_TIME */
        sleep(rand_r(&seed) % SLEEP_TIME + 1);

        // wait on empty
        sem_wait(&empty);

        // mutex lock
        pthread_mutex_lock(&mutex);

        /* critical section */
        printf("in producer critical section.\n");

        // generate a random number
        item = rand_r(&seed);

        // insert item into buffer and report on error condition if any
        if (insert_item(item) < 0) {
            printf("Could not insert into buffer. An item already exists!\n");

        }

        /* end critical section */

        // unlock mutex
        pthread_mutex_unlock(&mutex);

        // signal full
        sem_post(&full);

    }

}

/** Consumer Entry **/
void *consumer(void *param) {
    buffer_item *item = (int *)malloc(sizeof(int));
    while (1) {
        printf("in consumer.\n");
        /* Sleep for a random period of time between 1 and SLEEP_TIME */
        sleep(rand_r(&seed) % SLEEP_TIME + 1);

        // wait on full
        sem_wait(&full);

        // mutex lock
        pthread_mutex_lock(&mutex);

        /* critical section */
        printf("in consumer critical section.\n");

        // generate a random number
        //item = rand_r(&seed);

        // insert item into buffer and report on error condition if any
        if (remove_item(*item) < 0) {
            printf("Could not remove from buffer. The item does not exist!\n");
        }

        /* end critical section */

        // unlock mutex
        pthread_mutex_unlock(&mutex);

        // signal empty
        sem_post(&empty);

    }
    free(item);
}


int main(int argc, char*argv[]) {

    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    int producerCount;
    int consumerCount;

    // check if 3 args are passed
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        printf("Enter 3 arguments: sleep time, number of producers, number of consumers. \n");
        return -1;

    } else {
        SLEEP_TIME = atoi(argv[1]);
        producerCount = atoi(argv[2]);
        consumerCount = atoi(argv[3]);

    }

    /* 2. Initialize buffer, mutex, semaphores, and other global vars */
    seed = (unsigned int)time(NULL);
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    /* 3. Create producer thread(s) */
    pthread_t producers[producerCount];
    int i;
    for (i = 0; i < producerCount; i++) {
        pthread_attr_t pt_attr;

        // get default attributes
        pthread_attr_init(&pt_attr);

        // create thread 
        int producerResults = pthread_create(&producers[i], &pt_attr, producer, NULL);
        if (producerResults == -1) {
            printf("Error creating producer thread.\n");

        }

    }

    /* 4. Create consumer thread(s) */
    pthread_t consumers[consumerCount];
    for (i = 0; i < consumerCount; i++) {
        pthread_attr_t ct_attr;

        // get default attributes
        pthread_attr_init(&ct_attr);

        //create thread
        int consumerResults = pthread_create(&consumers[i], &ct_attr, consumer, NULL);
        if (consumerResults == -1) {
            printf("Error creating consumer thread.\n");

        }

    }

    /* 5. Sleep */
    //printf("Sleeping...\n");
    sleep(SLEEP_TIME);

    /* 6. Release resources, e.g. destroy mutex and semaphores */
    for(i = 0; i < producerCount; i++)
        pthread_cancel(producers[i]);
    for(i = 0; i < consumerCount; i++)
        pthread_cancel(consumers[i]);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    /* 7. Exit */
    return 0;

}

