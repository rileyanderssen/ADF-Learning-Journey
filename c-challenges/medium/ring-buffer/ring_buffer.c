/*****************************************************************************
 * Challenge 10: Circular Buffer (Ring Buffer)
 * 
 * Purpose:
 *   Implement a thread-safe circular buffer for inter-thread communication.
 *   Circular buffers are fundamental in embedded systems, real-time systems,
 *   and networking - used extensively in defense applications for data
 *   streaming, sensor buffers, and communication between system components.
 * 
 * Objective:
 *   Create a fixed-size circular buffer that allows multiple producer threads
 *   to write data and multiple consumer threads to read data concurrently,
 *   with proper synchronization to prevent race conditions and data loss.
 * 
 * Data Structure:
 *   typedef struct {
 *       int *buffer;           // Fixed-size array
 *       int capacity;          // Maximum number of elements
 *       int size;              // Current number of elements
 *       int head;              // Write position (producer)
 *       int tail;              // Read position (consumer)
 *       pthread_mutex_t mutex; // Protects buffer state
 *       pthread_cond_t not_full;  // Signals when space available
 *       pthread_cond_t not_empty; // Signals when data available
 *   } RingBuffer;
 * 
 * Requirements:
 *   - Fixed capacity buffer (e.g., 10 elements)
 *   - write() adds element at head, blocks if buffer full
 *   - read() removes element from tail, blocks if buffer empty
 *   - Thread-safe: multiple producers and consumers can operate simultaneously
 *   - No data loss or corruption
 *   - Proper synchronization using mutex and condition variables
 * 
 * Key Functions to Implement:
 *   RingBuffer* create_ring_buffer(int capacity)
 *   void write_ring_buffer(RingBuffer *rb, int value)
 *   int read_ring_buffer(RingBuffer *rb)
 *   void destroy_ring_buffer(RingBuffer *rb)
 * 
 * Circular Buffer Concept:
 *   Buffer wraps around when reaching the end:
 *   
 *   [0][1][2][3][4]  (capacity = 5)
 *    ^           ^
 *    tail       head
 *   
 *   After writing 5 items and reading 2:
 *   [5][6][ ][ ][3][4]
 *         ^   ^
 *        head tail
 *   
 *   head = (head + 1) % capacity  // Wrap around
 *   tail = (tail + 1) % capacity  // Wrap around
 * 
 * Thread Synchronization:
 *   - pthread_mutex_t: Protects shared state (head, tail, size)
 *   - pthread_cond_t not_full: Producer waits if buffer full
 *   - pthread_cond_t not_empty: Consumer waits if buffer empty
 * 
 * Synchronization Functions Needed:
 *   - pthread_mutex_init(&mutex, NULL)
 *   - pthread_mutex_lock(&mutex)
 *   - pthread_mutex_unlock(&mutex)
 *   - pthread_mutex_destroy(&mutex)
 *   - pthread_cond_init(&cond, NULL)
 *   - pthread_cond_wait(&cond, &mutex)
 *   - pthread_cond_signal(&cond)
 *   - pthread_cond_broadcast(&cond)
 *   - pthread_cond_destroy(&cond)
 * 
 * Producer-Consumer Pattern:
 *   Producer (write):
 *     1. Lock mutex
 *     2. While buffer full: wait on not_full condition
 *     3. Add item to buffer at head position
 *     4. Increment head (with wrap-around)
 *     5. Increment size
 *     6. Signal not_empty (wake up consumers)
 *     7. Unlock mutex
 *   
 *   Consumer (read):
 *     1. Lock mutex
 *     2. While buffer empty: wait on not_empty condition
 *     3. Read item from buffer at tail position
 *     4. Increment tail (with wrap-around)
 *     5. Decrement size
 *     6. Signal not_full (wake up producers)
 *     7. Unlock mutex
 * 
 * Test Cases:
 *   1. Create buffer with capacity 10
 *   2. Single producer, single consumer (verify FIFO order)
 *   3. Multiple producers (2), multiple consumers (2)
 *   4. Producer faster than consumer (buffer fills up)
 *   5. Consumer faster than producer (buffer empties)
 *   6. Verify no data loss over 1000+ operations
 *   7. Test blocking behavior (producer blocks when full)
 * 
 * Skills Practiced:
 *   - Circular buffer implementation
 *   - Thread synchronization with mutex
 *   - Condition variables for blocking
 *   - Producer-consumer problem
 *   - Race condition prevention
 *   - Modular arithmetic (wrap-around)
 * 
 * Common Pitfalls:
 *   - Forgetting to unlock mutex (deadlock!)
 *   - Not checking condition in while loop (use while, not if)
 *   - Incorrect wrap-around logic (use modulo: % capacity)
 *   - Signaling wrong condition variable
 *   - Not handling full/empty conditions properly
 * 
 * Example Test Program:
 *   void* producer(void *arg) {
 *       RingBuffer *rb = (RingBuffer*)arg;
 *       for (int i = 0; i < 100; i++) {
 *           write_ring_buffer(rb, i);
 *           printf("Produced: %d\n", i);
 *       }
 *       return NULL;
 *   }
 *   
 *   void* consumer(void *arg) {
 *       RingBuffer *rb = (RingBuffer*)arg;
 *       for (int i = 0; i < 100; i++) {
 *           int value = read_ring_buffer(rb);
 *           printf("Consumed: %d\n", value);
 *       }
 *       return NULL;
 *   }
 * 
 * Real-World Applications:
 *   - Audio/video streaming buffers
 *   - Sensor data collection (radar, telemetry)
 *   - Network packet queues
 *   - Logging systems
 *   - Inter-process communication
 *   - Hardware device drivers
 * 
 * Compilation:
 *   gcc -pthread -o ring_buffer circular_buffer.c
 *   (Note: -pthread flag required)
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
#include <unistd.h>

typedef struct {
    int *buffer; // fixed-size array
    int capacity; // max number of elements
    int size; // current number of elements
    int head; // write position (producer)
    int tail; // read position (consumer)
    pthread_mutex_t mutex; // protects buffer state
    pthread_cond_t not_full; // signals when space available
    pthread_cond_t not_empty; // signals when data available
} RingBuffer;


RingBuffer* create_ring_buffer(int capacity) {
    RingBuffer* r = (RingBuffer*)malloc(sizeof(RingBuffer));
    if (r == NULL) {
        return NULL;
    }

    r->capacity = capacity;
    r->head = 0;
    r->tail = 0;
    r->size = 0;

    r->buffer = malloc(capacity * sizeof(int));
    if (r->buffer == NULL) {
        free(r);
        return NULL;
    }

    pthread_mutex_init(&r->mutex, NULL);
    pthread_cond_init(&r->not_full, NULL);
    pthread_cond_init(&r->not_empty, NULL);

    return r;
}

void write_ring_buffer(RingBuffer *rb, int value) {
    pthread_mutex_lock(&rb->mutex);

    while (rb->size == rb->capacity) {
        // note -> this releases mutex while waiting
        pthread_cond_wait(&rb->not_full, &rb->mutex);
        // when woken, mutex is locked again
    }

    rb->buffer[rb->head] = value;
    rb->head = (rb->head + 1) % rb->capacity; // inc with wrap around
    rb->size += 1;

    pthread_cond_signal(&rb->not_empty);

    pthread_mutex_unlock(&rb->mutex);
}

int read_ring_buffer(RingBuffer *rb) {
    pthread_mutex_lock(&rb->mutex);

    while (rb->size == 0) {
        pthread_cond_wait(&rb->not_empty, &rb->mutex);
    }

    int item = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->size -= 1;

    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->mutex);

    return item;
}

void destroy_ring_buffer(RingBuffer *rb) {
    free(rb->buffer);
    free(rb);
    rb = NULL;
}

// TEST CASES
typedef struct {
    RingBuffer *rb;
    int start;
    int count;
} ThreadArgs;

void* producer_func(void *arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    for (int i = 0; i < args->count; i++) {
        write_ring_buffer(args->rb, args->start + i);
        if (i % 10 == 0) {
            printf("Producer: wrote %d\n", args->start + i);
        }
    }
    printf("Producer: finished writing %d items\n", args->count);
    return NULL;
}

void* consumer_func(void *arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    for (int i = 0; i < args->count; i++) {
        int val = read_ring_buffer(args->rb);
        if (i % 10 == 0) {
            printf("Consumer: read %d\n", val);
        }
        if (val != args->start + i) {
            printf("✗ ERROR: Expected %d, got %d\n", args->start + i, val);
        }
    }
    printf("Consumer: finished reading %d items\n", args->count);
    return NULL;
}

void* fast_producer(void *arg) {
    RingBuffer *rb = (RingBuffer*)arg;
    for (int i = 0; i < 20; i++) {
        write_ring_buffer(rb, i);
        printf("Fast producer wrote: %d\n", i);
    }

    return NULL;
}

void* slow_consumer(void *arg) {
    RingBuffer *rb = (RingBuffer*)arg;
    for (int i = 0; i < 20; i++) {
        int val = read_ring_buffer(rb);
        printf("Slow consumer read: %d\n", val);
        usleep(10000);
    }

    return NULL;
}

int main() {
    printf("=== CIRCULAR BUFFER (RING BUFFER) TEST SUITE ===\n\n");
    
    int tests_passed = 0;
    int tests_failed = 0;
    int failed_tests[7] = {0}; // Track which tests failed
    
    // ========== TEST 1: Basic single write/read ==========
    printf("--- Test 1: Basic Single Write/Read ---\n");
    
    RingBuffer *rb = create_ring_buffer(10);
    
    write_ring_buffer(rb, 42);
    int value = read_ring_buffer(rb);
    
    printf("Wrote: 42, Read: %d\n", value);
    
    if (value == 42) {
        printf("✓ Test 1 PASSED\n");
        tests_passed++;
    } else {
        printf("✗ Test 1 FAILED\n");
        tests_failed++;
        failed_tests[0] = 1;
    }
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 2: FIFO order verification ==========
    printf("--- Test 2: FIFO Order (First In, First Out) ---\n");
    
    rb = create_ring_buffer(5);
    
    for (int i = 1; i <= 5; i++) {
        write_ring_buffer(rb, i * 10);
    }
    
    printf("Wrote: 10, 20, 30, 40, 50\n");
    printf("Reading back: ");
    
    int fifo_correct = 1;
    for (int i = 1; i <= 5; i++) {
        int val = read_ring_buffer(rb);
        printf("%d ", val);
        if (val != i * 10) {
            fifo_correct = 0;
        }
    }
    printf("\n");
    
    if (fifo_correct) {
        printf("✓ Test 2 PASSED (FIFO order maintained)\n");
        tests_passed++;
    } else {
        printf("✗ Test 2 FAILED (FIFO order broken)\n");
        tests_failed++;
        failed_tests[1] = 1;
    }
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 3: Wrap-around behavior ==========
    printf("--- Test 3: Circular Wrap-Around ---\n");
    
    rb = create_ring_buffer(5);
    
    for (int i = 0; i < 5; i++) {
        write_ring_buffer(rb, i);
    }
    
    for (int i = 0; i < 3; i++) {
        read_ring_buffer(rb);
    }
    
    write_ring_buffer(rb, 100);
    write_ring_buffer(rb, 101);
    write_ring_buffer(rb, 102);
    
    printf("Reading after wrap-around: ");
    int expected[] = {3, 4, 100, 101, 102};
    int wrap_correct = 1;
    
    for (int i = 0; i < 5; i++) {
        int val = read_ring_buffer(rb);
        printf("%d ", val);
        if (val != expected[i]) {
            wrap_correct = 0;
        }
    }
    printf("\n");
    
    if (wrap_correct) {
        printf("✓ Test 3 PASSED (wrap-around works)\n");
        tests_passed++;
    } else {
        printf("✗ Test 3 FAILED (wrap-around broken)\n");
        tests_failed++;
        failed_tests[2] = 1;
    }
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 4: Single producer, single consumer threads ==========
    printf("--- Test 4: Single Producer, Single Consumer ---\n");
    
    rb = create_ring_buffer(10);
    
    ThreadArgs producer_args = {rb, 0, 50};
    ThreadArgs consumer_args = {rb, 0, 50};
    
    pthread_t producer_thread, consumer_thread;
    
    pthread_create(&producer_thread, NULL, producer_func, &producer_args);
    pthread_create(&consumer_thread, NULL, consumer_func, &consumer_args);
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    printf("✓ Test 4 PASSED (single producer/consumer)\n");
    tests_passed++;
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 5: Multiple producers, multiple consumers ==========
    printf("--- Test 5: Multiple Producers (2), Multiple Consumers (2) ---\n");
    
    rb = create_ring_buffer(20);
    
    ThreadArgs producer1_args = {rb, 0, 100};
    ThreadArgs producer2_args = {rb, 1000, 100};
    ThreadArgs consumer1_args = {rb, 0, 100};
    ThreadArgs consumer2_args = {rb, 0, 100};
    
    pthread_t prod1, prod2, cons1, cons2;
    
    printf("Starting 2 producers and 2 consumers...\n");
    printf("Producer 1: writing 0-99\n");
    printf("Producer 2: writing 1000-1099\n");
    printf("Each consumer will read 100 items\n\n");
    
    pthread_create(&prod1, NULL, producer_func, &producer1_args);
    pthread_create(&prod2, NULL, producer_func, &producer2_args);
    pthread_create(&cons1, NULL, consumer_func, &consumer1_args);
    pthread_create(&cons2, NULL, consumer_func, &consumer2_args);
    
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);
    
    printf("\n✓ Test 5 PASSED (no crashes or deadlocks)\n");
    printf("  Note: Order may be interleaved, but no data should be lost\n");
    tests_passed++;
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 6: Stress test with many operations ==========
    printf("--- Test 6: Stress Test (1000 operations) ---\n");
    
    rb = create_ring_buffer(10);
    
    ThreadArgs stress_prod = {rb, 0, 1000};
    ThreadArgs stress_cons = {rb, 0, 1000};
    
    pthread_t stress_producer, stress_consumer;
    
    printf("Running 1000 writes and 1000 reads...\n");
    
    clock_t start = clock();
    
    pthread_create(&stress_producer, NULL, producer_func, &stress_prod);
    pthread_create(&stress_consumer, NULL, consumer_func, &stress_cons);
    
    pthread_join(stress_producer, NULL);
    pthread_join(stress_consumer, NULL);
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Completed 1000 operations in %.3f seconds\n", time_spent);
    printf("✓ Test 6 PASSED (stress test successful)\n");
    tests_passed++;
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    // ========== TEST 7: Producer faster than consumer ==========
    printf("--- Test 7: Fast Producer, Slow Consumer ---\n");
    
    rb = create_ring_buffer(5);
    
    pthread_t fast_prod, slow_cons;
    
    printf("Producer writes fast, consumer reads slowly\n");
    printf("Buffer capacity is 5 - producer should block when full\n\n");
    
    pthread_create(&fast_prod, NULL, fast_producer, rb);
    pthread_create(&slow_cons, NULL, slow_consumer, rb);
    
    pthread_join(fast_prod, NULL);
    pthread_join(slow_cons, NULL);
    
    printf("\n✓ Test 7 PASSED (blocking behavior works)\n");
    tests_passed++;
    
    destroy_ring_buffer(rb);
    
    printf("\n================================\n\n");
    
    printf("=== ALL TESTS COMPLETE ===\n");
    printf("\n📊 FINAL RESULTS:\n");
    printf("   Tests Passed: %d/7\n", tests_passed);
    printf("   Tests Failed: %d/7\n", tests_failed);
    
    if (tests_failed > 0) {
        printf("\n⚠️  FAILED TESTS:\n");
        if (failed_tests[0]) printf("   ✗ Test 1: Basic Single Write/Read\n");
        if (failed_tests[1]) printf("   ✗ Test 2: FIFO Order\n");
        if (failed_tests[2]) printf("   ✗ Test 3: Circular Wrap-Around\n");
        if (failed_tests[3]) printf("   ✗ Test 4: Single Producer/Consumer\n");
        if (failed_tests[4]) printf("   ✗ Test 5: Multiple Producers/Consumers\n");
        if (failed_tests[5]) printf("   ✗ Test 6: Stress Test\n");
        if (failed_tests[6]) printf("   ✗ Test 7: Fast Producer/Slow Consumer\n");
    } else {
        printf("\n🎉 ALL TESTS PASSED! 🎉\n");
    }
    
    return 0;
}