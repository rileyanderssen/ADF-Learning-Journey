/*****************************************************************************
 * Challenge 7 (macOS): Simple Process Monitor
 * 
 * Purpose:
 *   Interact with the operating system to monitor running processes using
 *   the `ps` command. Understanding process management is critical for
 *   systems programming and debugging in defense applications.
 * 
 * Objective:
 *   Create a program that lists running processes, displays their information,
 *   and allows searching/filtering. Uses popen() to execute ps command and
 *   parse its output.
 * 
 * Data Structure:
 *   typedef struct {
 *       int pid;
 *       char name[256];
 *       float mem_percent;
 *   } Process;
 * 
 * Requirements:
 *   - Use popen() to run: "ps -eo pid,comm,%mem"
 *   - Parse output line by line with sscanf()
 *   - Store processes in dynamic array
 *   - Display: PID, name, memory usage for each process
 *   - Find and display top 5 processes by memory usage
 *   - Search for process by name (case-insensitive)
 * 
 * Key Functions to Implement:
 *   Process* get_all_processes(int *count)
 *   void print_top_n_by_memory(Process *procs, int count, int n)
 *   Process* find_process_by_name(Process *procs, int count, const char *name)
 * 
 * Test Cases:
 *   1. List all processes (verify count > 0)
 *   2. Display top 5 by memory
 *   3. Search for known process (e.g., "Finder")
 *   4. Search for non-existent process (handle gracefully)
 * 
 * Skills Practiced:
 *   - popen/pclose for running commands
 *   - Text parsing with sscanf
 *   - Dynamic memory allocation
 *   - Sorting with qsort
 *   - String comparison
 * 
 * Important Functions:
 *   - popen(command, "r") - Execute command, return FILE*
 *   - fgets(buffer, size, fp) - Read line from file
 *   - sscanf(line, "%d %s %f", &pid, name, &mem) - Parse line
 *   - qsort(array, count, size, comparator) - Sort array
 *   - strcasecmp(s1, s2) - Case-insensitive compare
 * 
 * Security Note:
 *   Never pass unsanitized user input to popen() - command injection risk!
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int pid;           
    char cmd[256];     
    float mem_percent; 
} Process;

int read_process(FILE *fp, Process *process) {
    if (fp == NULL || process == NULL) {
        return 0;
    }

    char line[512];

    // read line into buffer
    if (fgets(line, sizeof(line), fp) == NULL) {
        return 0; 
    }

    if (sscanf(line, "%d %s %f", &process->pid, process->cmd, &process->mem_percent) != 3) {
        return 0;
    }

    return 1;
}

Process* get_all_processes(int* count) {
    int num_processes = 0;

    FILE* fp = popen("ps -eo pid,comm,%mem", "r");

    if (fp == NULL) {
        printf("Error: fialed to run ps command\n");
        return NULL;
    }

    // skip header line
    char header[512];
    fgets(header, sizeof(header), fp);

    Process *processes = malloc(1000 * sizeof(Process));
    Process p;

    while (read_process(fp, &p) == 1) {
        processes[num_processes] = p;
        num_processes += 1;
    }

    pclose(fp);
    *count = num_processes;

    return processes;
}

// sorting function to sort by memory
int compare_by_memory(const void* a, const void* b) {
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;

    if (p2->mem_percent > p1->mem_percent) return 1;
    if (p2->mem_percent < p1->mem_percent) return -1;

    return 0;
}

void print_top_n_by_memory(Process *procs, int count, int n) {
    qsort(procs, count, sizeof(Process), compare_by_memory);

    for (int i = 0; i < n; i++) {
        printf("Order number: %d\n", i);
        printf("Pid: %d\n", procs[i].pid);
        printf("Cmd: %s\n", procs[i].cmd);
        printf("Memory: %f\n\n", procs[i].mem_percent);
    }
}


Process* find_process_by_name(Process *procs, int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcasestr(procs[i].cmd, name) != NULL) {
            return &procs[i];
        }
    }
    
    return NULL;
}   

int main() {
    printf("=== PROCESS MONITOR TEST SUITE ===\n\n");
    
    // ========== TEST 1: List all processes (verify count > 0) ==========
    printf("--- Test 1: Get All Processes ---\n");
    int count;
    Process *procs = get_all_processes(&count);
    
    if (procs == NULL) {
        printf("ERROR: Failed to get processes\n");
        return 1;
    }
    
    printf("Total processes found: %d\n", count);
    
    if (count > 0) {
        printf("✓ Test 1 PASSED: Found %d processes\n", count);
    } else {
        printf("✗ Test 1 FAILED: No processes found\n");
        free(procs);
        return 1;
    }
    
    printf("\nFirst 10 processes:\n");
    printf("%-8s %-40s %8s\n", "PID", "COMMAND", "%MEM");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < 10 && i < count; i++) {
        printf("%-8d %-40s %7.1f%%\n", 
               procs[i].pid, 
               procs[i].cmd, 
               procs[i].mem_percent);
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 2: Display top 5 by memory ==========
    printf("--- Test 2: Top 5 Processes by Memory Usage ---\n");
    print_top_n_by_memory(procs, count, 5);
    printf("✓ Test 2 COMPLETED\n");
    printf("\n================================\n\n");
    
    // ========== TEST 3: Search for known process ==========
    printf("--- Test 3: Search for Known Process (Finder) ---\n");
    Process *found = find_process_by_name(procs, count, "Finder");
    
    if (found != NULL) {
        printf("✓ Found process:\n");
        printf("  PID: %d\n", found->pid);
        printf("  Command: %s\n", found->cmd);
        printf("  Memory: %.1f%%\n", found->mem_percent);
        printf("✓ Test 3 PASSED\n");
    } else {
        printf("⚠ Process 'Finder' not found (might not be running)\n");
        printf("⚠ Test 3 INCONCLUSIVE\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 3b: Search for another known process ==========
    printf("--- Test 3b: Search for Known Process (kernel_task) ---\n");
    found = find_process_by_name(procs, count, "kernel_task");
    
    if (found != NULL) {
        printf("✓ Found process:\n");
        printf("  PID: %d\n", found->pid);
        printf("  Command: %s\n", found->cmd);
        printf("  Memory: %.1f%%\n", found->mem_percent);
        printf("✓ Test 3b PASSED\n");
    } else {
        printf("✗ Process 'kernel_task' not found (should always be running)\n");
        printf("✗ Test 3b FAILED\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 4: Search for non-existent process ==========
    printf("--- Test 4: Search for Non-Existent Process ---\n");
    found = find_process_by_name(procs, count, "ThisProcessDoesNotExist12345");
    
    if (found == NULL) {
        printf("✓ Correctly returned NULL for non-existent process\n");
        printf("✓ Test 4 PASSED: Handled gracefully\n");
    } else {
        printf("✗ Test 4 FAILED: Should return NULL for non-existent process\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== Additional Test: Edge Cases ==========
    printf("--- Additional Test: Edge Cases ---\n");
    
    // Test searching for empty string
    printf("Searching for empty string: ");
    found = find_process_by_name(procs, count, "");
    if (found != NULL) {
        printf("Found (expected - should match something)\n");
    } else {
        printf("Not found\n");
    }
    
    // Test top N with different values
    printf("\nTop 3 processes:\n");
    print_top_n_by_memory(procs, count, 3);
    
    printf("\nTop 1 process:\n");
    print_top_n_by_memory(procs, count, 1);
    
    printf("\n================================\n\n");
    
    // ========== Cleanup ==========
    printf("--- Cleanup ---\n");
    free(procs);
    printf("✓ Memory freed\n");
    
    printf("\n=== ALL TESTS COMPLETE ===\n");
    
    return 0;
}