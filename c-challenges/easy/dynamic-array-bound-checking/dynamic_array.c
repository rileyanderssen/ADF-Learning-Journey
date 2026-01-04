/*****************************************************************************
 * Challenge 3: Dynamic Array with Bounds Checking
 * 
 * Purpose:
 *   Implement a safe, resizable array using dynamic memory allocation to
 *   understand manual memory management and defensive programming practices -
 *   critical skills for writing robust C programs that handle runtime data
 *   of unknown size.
 * 
 * Objective:
 *   Create a dynamic array structure that grows automatically when needed,
 *   enforces bounds checking to prevent memory errors, and properly manages
 *   heap-allocated memory. This demonstrates fundamental data structure
 *   implementation and memory safety principles essential in systems
 *   programming and defense software where reliability is paramount.
 * 
 * Data Structure:
 *   typedef struct {
 *       int *data;           // Pointer to heap-allocated array
 *       size_t size;         // Current number of elements
 *       size_t capacity;     // Total allocated capacity
 *   } DynArray;
 * 
 * Function Signatures:
 *   DynArray* create_array(size_t initial_capacity)
 *   void append(DynArray *arr, int value)
 *   int get(DynArray *arr, size_t index, int *error)
 *   void resize(DynArray *arr, size_t new_capacity)
 *   void destroy_array(DynArray *arr)
 * 
 * Requirements:
 *   - create_array: Allocate a new dynamic array with given initial capacity
 *                   Return NULL if allocation fails
 *   - append: Add element to end of array
 *             Automatically double capacity if array is full
 *   - get: Retrieve element at index with bounds checking
 *          Set *error to 1 if index out of bounds, 0 if successful
 *   - resize: Change array capacity (grow or shrink)
 *             Preserve existing elements, handle realloc failures
 *   - destroy_array: Free all allocated memory (data array + struct)
 *                    Prevent memory leaks and double-free errors
 * 
 * Test Cases:
 *   1. Create array and verify initial state (size=0, capacity set)
 *   2. Append elements and verify they're stored correctly
 *   3. Trigger automatic resize by filling beyond initial capacity
 *   4. Test bounds checking with valid and invalid indices
 *   5. Manually resize array (both grow and shrink)
 *   6. Test error handling (NULL pointers, allocation failures)
 *   7. Verify proper cleanup with destroy_array (use valgrind if available)
 * 
 * Skills Practiced:
 *   - Dynamic memory allocation: malloc(), calloc()
 *   - Memory reallocation: realloc()
 *   - Memory deallocation: free()
 *   - Pointer manipulation and dereferencing
 *   - Struct design and usage
 *   - Error handling and validation
 *   - Bounds checking and defensive programming
 *   - Memory leak prevention
 *   - Understanding the heap vs stack
 * 
 * Memory Safety Principles:
 *   This challenge teaches critical memory management rules:
 *   1. Always check if malloc/realloc returns NULL (allocation can fail)
 *   2. Never access memory outside allocated bounds
 *   3. Always free() dynamically allocated memory when done
 *   4. Never free() the same pointer twice (double-free error)
 *   5. Don't use pointers after they've been freed (use-after-free)
 *   6. Match every malloc/calloc with exactly one free
 * 
 * Real-World Applications:
 *   Dynamic arrays are fundamental to many real-world systems:
 *   - Log collection systems (unknown number of events)
 *   - Sensor data buffers (variable incoming data)
 *   - Network packet queues
 *   - Database result sets
 *   - Command history in CLI applications
 *   - Vector/ArrayList implementations in higher-level languages
 *   
 *   In defense systems, proper memory management prevents:
 *   - Memory exhaustion attacks
 *   - Buffer overflow vulnerabilities
 *   - System crashes from out-of-memory conditions
 *   - Memory leaks causing degraded performance over time
 * 
 * Example Usage:
 *   DynArray *arr = create_array(2);    // Start with capacity 2
 *   append(arr, 10);                     // [10], size=1, capacity=2
 *   append(arr, 20);                     // [10, 20], size=2, capacity=2
 *   append(arr, 30);                     // [10, 20, 30], auto-resize to capacity=4
 *   
 *   int error = 0;
 *   int val = get(arr, 1, &error);      // val=20, error=0 (success)
 *   val = get(arr, 5, &error);          // error=1 (out of bounds)
 *   
 *   destroy_array(arr);                  // Free all memory, prevent leaks
 * 
 * Debugging Tips:
 *   - Use valgrind to detect memory leaks: valgrind --leak-check=full ./program
 *   - Print size and capacity after each operation during development
 *   - Initialize freed pointers to NULL to catch use-after-free bugs
 *   - Test with small initial capacities to trigger resize behavior
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int *data;
    size_t size; // elements currently used
    size_t capacity; // total elements that can be put in
} DynArray;

DynArray* create_array(size_t initial_capacity) {
    // allocate memory for one DynArray struct
    DynArray *arr = (DynArray*)malloc(sizeof(DynArray));

    // make sure malloc worked
    if (arr == NULL) {
        return NULL;
    }

    // set the capacity and size 
    arr->capacity = initial_capacity;
    arr->size = 0;

    // allocate memory for initial capacity
    // ... storing ints, so * size of int
    arr->data = (int*)malloc(initial_capacity * sizeof(int));

    // make sure data part memory allocation worked
    if (arr->data == NULL) {
        free(arr); 
        return NULL;
    }

    return arr;
}

void resize(DynArray *arr, size_t new_capacity) {
    if (arr == NULL) {
        return;
    }

    int new_cap_bytes = new_capacity * sizeof(int);
    // store realloc return value in a temp var so data is not lost if it fails
    int *temp = realloc(arr->data, new_cap_bytes);

    if (temp == NULL) {
        // it failed
        printf("Realloc failed");
        return;
    }

    arr->data = temp;
    arr->capacity = new_capacity;

    if (arr->size > new_capacity) {
        arr->size = new_capacity;
    }
}

void append(DynArray *arr, int value) {
    if (arr == NULL) {
        return;
    }

    // check if it can be added first
    if (arr->size == arr->capacity) {
        resize(arr, arr->capacity * 2);
    }

    arr->data[arr->size] = value;
    arr->size += 1;
}

int get(DynArray *arr, size_t index, int *error) {
    if (arr == NULL) {
        *error = 1;
        return 0;
    }

    if (index >= arr->capacity || index >= arr->size || index < 0) {
        // set the error as out of bounds was requested for
        *error = 1;
        return 0;
    }

    // return the value
    *error = 0;
    return arr->data[index];
}

void destroy_array(DynArray *arr) {
    if (arr == NULL) {
        return;
    }

    free(arr->data);
    free(arr);
}

// function for testing
void print_array_data(DynArray *arr) {
    printf("Size: %zu, Capacity: %zu\n", arr->size, arr->capacity);

    printf("Data: [");

    for (size_t i = 0; i < arr->size; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) {
            printf(", ");
        }
    }
    
    printf("]\n");
}

int main() {
    printf("=== Testing Dynamic Array with Bounds Checking ===\n\n");
    
    // Test Case 1: Create array and verify initial state
    printf("Test 1: Create array and verify initial state\n");
    DynArray *arr = create_array(3);
    if (arr == NULL) {
        printf("  FAIL ✗ - create_array returned NULL\n\n");
        return 1;
    }
    printf("  Initial state: ");
    print_array_data(arr);
    printf("  Expected: Size=0, Capacity=3\n");
    printf("  Status: %s\n\n", 
           (arr->size == 0 && arr->capacity == 3) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 2: Append elements and verify they're stored correctly
    printf("Test 2: Append elements and verify storage\n");
    append(arr, 10);
    append(arr, 20);
    append(arr, 30);
    printf("  After appending 10, 20, 30: ");
    print_array_data(arr);
    
    int error;
    int val1 = get(arr, 0, &error);
    int val2 = get(arr, 1, &error);
    int val3 = get(arr, 2, &error);
    
    printf("  Values: [%d, %d, %d]\n", val1, val2, val3);
    printf("  Expected: [10, 20, 30]\n");
    printf("  Status: %s\n\n", 
           (val1 == 10 && val2 == 20 && val3 == 30 && arr->size == 3) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 3: Trigger automatic resize by filling beyond initial capacity
    printf("Test 3: Automatic resize when capacity exceeded\n");
    printf("  Before 4th append: ");
    print_array_data(arr);
    printf("  Capacity before: %zu\n", arr->capacity);
    
    append(arr, 40);  // Should trigger resize (capacity 3 -> 6)
    
    printf("  After 4th append: ");
    print_array_data(arr);
    printf("  Capacity after: %zu\n", arr->capacity);
    printf("  Expected: Capacity doubled to 6, size=4\n");
    printf("  Status: %s\n\n", 
           (arr->capacity == 6 && arr->size == 4) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 4: Test bounds checking with valid and invalid indices
    printf("Test 4: Bounds checking\n");
    
    // Valid index
    int valid_val = get(arr, 2, &error);
    printf("  get(arr, 2): value=%d, error=%d (expected: 30, 0)\n", valid_val, error);
    int valid_pass = (valid_val == 30 && error == 0);
    
    // Invalid index (out of bounds)
    int invalid_val = get(arr, 10, &error);
    printf("  get(arr, 10): value=%d, error=%d (expected: any, 1)\n", invalid_val, error);
    int invalid_pass = (error == 1);
    
    // Boundary case (exactly at size)
    int boundary_val = get(arr, arr->size, &error);
    printf("  get(arr, %zu): value=%d, error=%d (expected: any, 1)\n", arr->size, boundary_val, error);
    int boundary_pass = (error == 1);
    
    printf("  Status: %s\n\n", 
           (valid_pass && invalid_pass && boundary_pass) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 5: Manually resize array (both grow and shrink)
    printf("Test 5: Manual resize (grow and shrink)\n");
    
    // Grow
    printf("  Before resize to 10: ");
    print_array_data(arr);
    resize(arr, 10);
    printf("  After resize to 10: ");
    print_array_data(arr);
    printf("  Capacity: %zu (expected: 10)\n", arr->capacity);
    int grow_pass = (arr->capacity == 10 && arr->size == 4);
    
    // Verify data preserved
    int preserved = (get(arr, 0, &error) == 10 && 
                    get(arr, 1, &error) == 20 && 
                    get(arr, 2, &error) == 30 && 
                    get(arr, 3, &error) == 40);
    printf("  Data preserved: %s\n", preserved ? "YES ✓" : "NO ✗");
    
    // Shrink
    printf("  Before resize to 2: ");
    print_array_data(arr);
    resize(arr, 2);
    printf("  After resize to 2: ");
    print_array_data(arr);
    printf("  Capacity: %zu, Size: %zu (expected: 2, 2)\n", arr->capacity, arr->size);
    int shrink_pass = (arr->capacity == 2 && arr->size == 2);
    
    printf("  Status: %s\n\n", 
           (grow_pass && shrink_pass && preserved) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 6: Test error handling (NULL pointers)
    printf("Test 6: Error handling with NULL pointers\n");
    
    // Test get with NULL array
    int null_val = get(NULL, 0, &error);
    printf("  get(NULL, 0): error=%d (expected: 1)\n", error);
    int null_get_pass = (error == 1);
    
    // Test append with NULL (shouldn't crash)
    printf("  append(NULL, 100): ");
    append(NULL, 100);
    printf("Should not crash ✓\n");
    
    // Test resize with NULL (shouldn't crash)
    printf("  resize(NULL, 10): ");
    resize(NULL, 10);
    printf("Should not crash ✓\n");
    
    // Test destroy with NULL (shouldn't crash)
    printf("  destroy_array(NULL): ");
    destroy_array(NULL);
    printf("Should not crash ✓\n");
    
    printf("  Status: %s\n\n", null_get_pass ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 7: Verify proper cleanup
    printf("Test 7: Proper cleanup with destroy_array\n");
    printf("  Current array: ");
    print_array_data(arr);
    printf("  Destroying array...\n");
    destroy_array(arr);
    printf("  Array destroyed successfully ✓\n");
    printf("  Status: PASS ✓ (run with valgrind to verify no leaks)\n\n");
    
    // Additional test: Create array with capacity 0
    printf("Additional Test: Edge case - capacity 0\n");
    DynArray *arr2 = create_array(0);
    if (arr2 != NULL) {
        printf("  Created with capacity 0: ");
        print_array_data(arr2);
        
        printf("  Appending to empty array...\n");
        append(arr2, 100);
        printf("  After first append: ");
        print_array_data(arr2);
        
        append(arr2, 200);
        printf("  After second append: ");
        print_array_data(arr2);
        
        printf("  Status: %s\n", (arr2->size == 2) ? "PASS ✓" : "FAIL ✗");
        
        destroy_array(arr2);
    } else {
        printf("  Array creation with capacity 0 returned NULL\n");
    }
    
    printf("\n=== All tests complete ===\n");
    
    return 0;
}