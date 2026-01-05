/*****************************************************************************
 * Challenge 8: Multi-threaded File Hasher
 * 
 * Purpose:
 *   Learn concurrent programming by implementing a multi-threaded file hasher.
 *   Understanding threads and synchronization is critical for modern defense
 *   systems that process large amounts of data in parallel.
 * 
 * Objective:
 *   Create a program that splits a file into chunks, computes a hash for
 *   each chunk in parallel using multiple threads, then combines the results.
 *   Compare single-threaded vs multi-threaded performance to understand
 *   the benefits and challenges of concurrency.
 * 
 * Data Structures:
 *   typedef struct {
 *       uint8_t *data;      // Chunk data
 *       size_t size;        // Chunk size
 *       uint64_t hash;      // Hash result for this chunk
 *       int chunk_id;       // Chunk identifier
 *   } Chunk;
 * 
 * Requirements:
 *   - Read a large file and split into equal-sized chunks
 *   - Create N threads (e.g., 4 threads)
 *   - Each thread computes hash for assigned chunks
 *   - Use simple hash: sum of all bytes (uint64_t)
 *   - Combine chunk hashes into final hash
 *   - Use pthread_mutex_t to protect shared data
 *   - Measure and compare execution time: 1 thread vs 4 threads
 * 
 * Key Functions to Implement:
 *   uint64_t hash_chunk(uint8_t *data, size_t size)
 *   void* worker_thread(void *arg)
 *   uint64_t hash_file_single_threaded(const char *filename)
 *   uint64_t hash_file_multi_threaded(const char *filename, int num_threads)
 * 
 * Pthread Functions Needed:
 *   - pthread_create(&thread, NULL, worker_thread, arg)
 *   - pthread_join(thread, NULL)
 *   - pthread_mutex_init(&mutex, NULL)
 *   - pthread_mutex_lock(&mutex)
 *   - pthread_mutex_unlock(&mutex)
 *   - pthread_mutex_destroy(&mutex)
 * 
 * Test Cases:
 *   1. Create a large test file (10MB+)
 *   2. Hash with 1 thread and measure time
 *   3. Hash with 4 threads and measure time
 *   4. Verify both methods produce same hash
 *   5. Test with different file sizes
 *   6. Test with different thread counts (1, 2, 4, 8)
 * 
 * Skills Practiced:
 *   - Thread creation and management
 *   - Mutex synchronization
 *   - Race condition prevention
 *   - Performance measurement
 *   - File I/O with large files
 *   - Memory management in concurrent code
 * 
 * Race Condition Example (What NOT to do):
 *   uint64_t total_hash = 0;  // Shared variable
 *   
 *   // Thread 1 and Thread 2 both do this:
 *   total_hash += chunk_hash;  // RACE CONDITION!
 *   
 *   // Solution: Use mutex
 *   pthread_mutex_lock(&mutex);
 *   total_hash += chunk_hash;
 *   pthread_mutex_unlock(&mutex);
 * 
 * Performance Measurement:
 *   #include <time.h>
 *   
 *   clock_t start = clock();
 *   // ... do work ...
 *   clock_t end = clock();
 *   double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
 * 
 * Compilation:
 *   gcc -pthread -o hasher multi_threaded_hasher.c
 *   (Note: -pthread flag is required for pthreads)
 * 
 * Expected Performance:
 *   On a 4-core system with 10MB file:
 *   - 1 thread:  ~X seconds
 *   - 4 threads: ~X/3 seconds (not X/4 due to overhead)
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

// data structure - each thread needs to know this data
typedef struct {
    uint8_t *data; // ... pointer to start of this chunk
    size_t size; // ... bytes in this chunk
    uint64_t hash; // ... outputs result here
    int thread_id; // ... for debugging if needed
} ThreadData;

uint64_t hash_chunk(uint8_t *data, size_t size) {
    uint64_t hash = 0;

    for (int i = 0; i < size; i++) {
        hash += data[i];
    }

    return hash;
}

void* worker_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("Thread %d is completing hash\n", data->thread_id);

    uint64_t hash = hash_chunk(data->data, data->size);

    data->hash = hash;

    return NULL;
}

uint64_t hash_file_single_threaded(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: Cannot open file\n");
        return 0;
    }

    // find file size to allocate the correct memory
    fseek(fp, 0, SEEK_END); // go to end of file
    long file_size = ftell(fp); // get the position
    fseek(fp, 0, SEEK_SET); // reset position to the start

    printf("The file size is: %ld bytes\n", file_size);

    // allocate the required amount of memory
    uint8_t *buffer =  malloc(file_size);
    if (buffer == NULL) {
        printf("Error: out of memory\n");
        fclose(fp);
        return 0;
    }

    // now read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, fp);
    if (bytes_read != file_size) {
        printf("Error: Could not read entire file\n");
        free(buffer);
        fclose(fp);
        return 0;
    }

    // now compute the hash
    uint64_t hash = 0;
    for (size_t i = 0; i < file_size; i++) {
        hash += buffer[i];
    }

    free(buffer);
    fclose(fp);

    return hash;
}

uint64_t hash_file_multi_threaded(const char *filename, int num_threads) {
    // first, determine the file size
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: Cannot open file\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END); 
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET); 

    // allocate the required amount of memory
    uint8_t *buffer =  malloc(file_size);
    if (buffer == NULL) {
        printf("Error: out of memory\n");
        fclose(fp);
        return 0;
    }

    // now read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, fp);
    if (bytes_read != file_size) {
        printf("Error: Could not read entire file\n");
        free(buffer);
        fclose(fp);
        return 0;
    }

    // next, determine the start position for each thread
    size_t i = 0;
    int chunk_size = (file_size + num_threads - 1) / num_threads;

    // set up thread data array
    ThreadData thread_data[num_threads];
    pthread_t threads[num_threads];

    while (i < num_threads) {
        int final_chunk_size = chunk_size;

        // get the start position in the buffer for this chunk
        int start_pos = chunk_size * i;
        // then calc the end position of this chunk based off default chunk size
        int end_pos = start_pos + chunk_size; 

        if (end_pos > file_size) {
            final_chunk_size = file_size - start_pos;
        }

        thread_data[i].data = buffer + chunk_size * i;
        thread_data[i].thread_id = i;
        thread_data[i].size = final_chunk_size;

        i++;
    }

    // create threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_thread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    uint64_t final_hash = 0;
    for (int i = 0; i < num_threads; i++) {
        final_hash += thread_data[i].hash;
    }

    return final_hash;
}

int main() {
    clock_t start = clock();
    
    uint64_t hash = hash_file_single_threaded("test_file.bin");
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Hash: %llu\n", hash);
    printf("Time: %.3f seconds\n", time_spent);

    clock_t start_parallel = clock();

    uint64_t hash_parallel = hash_file_multi_threaded("test_file.bin", 3);

    clock_t end_parallel = clock();
    double time_spent_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    printf("Hash: %llu\n", hash_parallel);
    printf("Time: %.3f seconds\n", time_spent_parallel);
    
    return 0;
}

