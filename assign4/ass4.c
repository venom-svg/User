#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int availableSeats = 0; // Will be set by user input
int useSynchronization = 0; // 0 = No sync, 1 = With sync
pthread_rwlock_t rwlock; //rwlock: read-write lock (multiple readers OR one writer at a time).
pthread_mutex_t writeMutex; //writeMutex: ensures only one writer enters critical section.


// Reader thread function
void* reader(void* arg) {
    int reader_id = *(int*)arg;  //Extract reader_id from passed pointer.
    usleep(rand() % 50000); // Random delay

    switch (useSynchronization) {
        case 1: // With synchronization
            pthread_rwlock_rdlock(&rwlock);  //Acquires read lock so multiple readers can read safely.
            printf("Reader %d is reading. Available seats: %d\n", reader_id, availableSeats);
            pthread_rwlock_unlock(&rwlock);
            break;
            
        case 0: // Without synchronization with preemption
        default:
            printf("Reader %d is trying to read available seats.\n", reader_id);
            // Simulate preemption: read value, then delay before printing
            int localSeats = availableSeats;
            // Simulate thread being preempted (giving other threads a chance to run)
            usleep(rand() % 20000);
            printf("Reader %d: Available seats = %d (Current actual seats: %d)\n",
                   reader_id, localSeats, availableSeats);
            break;
    }
    
    free(arg);
    return NULL;
}

// Writer thread function
void* writer(void* arg) {
    int writer_id = *(int*)arg;
    usleep(rand() % 50000); // Random delay

    switch (useSynchronization) {
        case 1: // With synchronization
            pthread_mutex_lock(&writeMutex);
            pthread_rwlock_wrlock(&rwlock);
            if (availableSeats > 0) {
                availableSeats--;
                printf("Writer %d reserved a seat. Remaining seats: %d\n",
                       writer_id, availableSeats);
            } else {
                printf("Writer %d attempted to reserve a seat, but no seats are available.\n",
                       writer_id);
            }
            pthread_rwlock_unlock(&rwlock);
            pthread_mutex_unlock(&writeMutex);
            break;
            
        case 0: // Without synchronization with preemption
        default:
            printf("Writer %d is trying to reserve a seat.\n", writer_id);
            // First check if seats are available
            if (availableSeats > 0) {
                // Read current value
                int localSeats = availableSeats;
                // Simulate preemption (giving other threads a chance to run)
                usleep(rand() % 20000);
                // Update the value (might be outdated now due to preemption)
                availableSeats = localSeats - 1;
                printf("Writer %d: Seat reserved. Remaining seats = %d\n",
                       writer_id, availableSeats);
            } else {
                printf("Writer %d: No seats available.\n", writer_id);
            }
            break;
    }
    
    free(arg);
    return NULL;
}

int main() {
    srand(time(NULL));
    int numReaders, numWriters;
    
    // Get user input for available seats
    printf("Enter number of available seats: ");
    scanf("%d", &availableSeats);
    
    // Get user input for number of readers
    printf("Enter number of reader threads: ");
    scanf("%d", &numReaders);
    
    // Get user input for number of writers
    printf("Enter number of writer threads: ");
    scanf("%d", &numWriters);
    
    // Get synchronization choice
    printf("Choose Mode:\n1. Without Synchronization (with preemption)\n2. With Synchronization\nEnter choice: ");
    int syncChoice;
    scanf("%d", &syncChoice);
    
    switch (syncChoice) {
        case 2:
            useSynchronization = 1;
            printf("Running with synchronization...\n");
            break;
        case 1:
        default:
            useSynchronization = 0;
            printf("Running without synchronization (with preemption)...\n");
            break;
    }
    
    // Calculate total number of threads
    int totalThreads = numReaders + numWriters;
    pthread_t* threads = (pthread_t*)malloc(totalThreads * sizeof(pthread_t));
    //Allocate memory to store thread IDs.
    int thread_count = 0;
    
    if (useSynchronization) {
        pthread_rwlock_init(&rwlock, NULL);
        pthread_mutex_init(&writeMutex, NULL);
    }
    
    // Create reader threads  Creates reader threads and assigns unique IDs.
    for (int i = 0; i < numReaders; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&threads[thread_count++], NULL, reader, id);
    }
    
    // Create writer threads  Creates writer threads similarly.
    for (int i = 0; i < numWriters; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&threads[thread_count++], NULL, writer, id);
    }
    
    // Wait for all threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    if (useSynchronization) {
        pthread_rwlock_destroy(&rwlock);
        pthread_mutex_destroy(&writeMutex);
    }
    
    free(threads);
    printf("Final number of available seats: %d\n", availableSeats);
    
    return 0;
}
