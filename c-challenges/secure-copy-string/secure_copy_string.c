/*****************************************************************************
 * Challenge 1: Secure String Copy
 * 
 * Purpose:
 *   Implement a buffer-overflow-safe string copy function to understand
 *   secure string handling and memory boundaries.
 * 
 * Objective:
 *   Write safe_strcpy() that copies a source string to a destination buffer
 *   while preventing buffer overflows - a critical security skill for
 *   defense software engineering.
 * 
 * Function Signature:
 *   size_t safe_strcpy(char *dest, const char *src, size_t dest_size)
 * 
 * Requirements:
 *   - Copy src to dest without exceeding dest_size
 *   - Always null-terminate the destination
 *   - Return number of characters copied (excluding null terminator)
 *   - Handle edge cases gracefully
 * 
 * Test Cases:
 *   1. Normal copy where src fits in dest
 *   2. src longer than dest (should truncate safely)
 *   3. Empty string ("")
 *   4. dest_size of 0 (should not write anything)
 *   5. dest_size of 1 (should only write null terminator)
 * 
 * Skills Practiced:
 *   - Pointers and pointer arithmetic
 *   - Array bounds checking
 *   - Buffer overflow prevention
 *   - Defensive programming
 *   - Security-aware coding practices
 * 
 * Security Note:
 *   Buffer overflows are one of the most common vulnerabilities in C programs.
 *   Understanding how to prevent them is essential for writing secure code,
 *   especially in defense systems where security is paramount.
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stddef.h>
#include <stdio.h>

// Note -> size_t is a standard unsigned integer type used for sizes and counts

size_t safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (dest_size == 0) {
        return 0;
    }

    size_t i = 0;

    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0'; // C has no built in string length, null terminator allows for C to know end char

    return i;
}

int main() {
    printf("=== Testing safe_strcpy() ===\n\n");
    
    // Test Case 1: Normal copy where src fits in dest
    printf("Test 1: Normal copy (src fits in dest)\n");
    char test1[20];
    size_t result1 = safe_strcpy(test1, "Hello World", sizeof(test1));
    printf("  Source: \"Hello World\"\n");
    printf("  Result: \"%s\"\n", test1);
    printf("  Characters copied: %zu\n", result1);
    printf("  Expected: 11 characters, \"Hello World\"\n");
    printf("  Status: %s\n\n", (result1 == 11) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 2: src longer than dest (should truncate safely)
    printf("Test 2: Truncation (src longer than dest)\n");
    char test2[8];
    size_t result2 = safe_strcpy(test2, "This is a very long string", sizeof(test2));
    printf("  Source: \"This is a very long string\"\n");
    printf("  Dest size: 8 bytes\n");
    printf("  Result: \"%s\"\n", test2);
    printf("  Characters copied: %zu\n", result2);
    printf("  Expected: 7 characters, \"This is\" (truncated)\n");
    printf("  Status: %s\n\n", (result2 == 7) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 3: Empty string ("")
    printf("Test 3: Empty string\n");
    char test3[10];
    size_t result3 = safe_strcpy(test3, "", sizeof(test3));
    printf("  Source: \"\"\n");
    printf("  Result: \"%s\"\n", test3);
    printf("  Characters copied: %zu\n", result3);
    printf("  Expected: 0 characters, empty string\n");
    printf("  Status: %s\n\n", (result3 == 0) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 4: dest_size of 0 (should not write anything)
    printf("Test 4: dest_size = 0 (should not write anything)\n");
    char test4[10] = "UNCHANGED";  // Pre-fill to verify no write
    size_t result4 = safe_strcpy(test4, "Hello", 0);
    printf("  Source: \"Hello\"\n");
    printf("  Dest size: 0\n");
    printf("  Characters copied: %zu\n", result4);
    printf("  Buffer unchanged: %s\n", test4);
    printf("  Expected: 0 characters copied, buffer unchanged\n");
    printf("  Status: %s\n\n", (result4 == 0) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 5: dest_size of 1 (should only write null terminator)
    printf("Test 5: dest_size = 1 (only null terminator)\n");
    char test5[10];
    size_t result5 = safe_strcpy(test5, "Hello", 1);
    printf("  Source: \"Hello\"\n");
    printf("  Dest size: 1\n");
    printf("  Result: \"%s\"\n", test5);
    printf("  Characters copied: %zu\n", result5);
    printf("  Expected: 0 characters (only null terminator)\n");
    printf("  Status: %s\n\n", (result5 == 0) ? "PASS ✓" : "FAIL ✗");
    
    printf("=== All tests complete ===\n");
    
    return 0;
}