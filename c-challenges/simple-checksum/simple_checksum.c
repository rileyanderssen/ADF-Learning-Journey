/*****************************************************************************
 * Challenge 5: Simple Checksum Implementation
 * 
 * Purpose:
 *   Implement multiple checksum algorithms to understand data integrity
 *   verification - critical for defense communication systems and data
 *   transmission where ensuring data hasn't been corrupted is essential.
 * 
 * Objective:
 *   Create three different checksum algorithms and demonstrate their
 *   ability to detect data corruption. Checksums provide quick integrity
 *   verification for transmitted or stored data.
 * 
 * Function Signatures:
 *   uint8_t simple_checksum(const uint8_t *data, size_t len)
 *   uint8_t xor_checksum(const uint8_t *data, size_t len)
 *   uint16_t fletcher16(const uint8_t *data, size_t len)
 * 
 * Requirements:
 *   - simple_checksum: Add all bytes together, return lowest 8 bits
 *   - xor_checksum: XOR all bytes together
 *   - fletcher16: Position-dependent checksum
 *                 sum1 = (sum1 + data[i]) % 255
 *                 sum2 = (sum2 + sum1) % 255
 *                 result = (sum2 << 8) | sum1
 * 
 * Test Cases:
 *   1. Compute checksum of a test message
 *   2. Corrupt one byte in the message
 *   3. Verify checksum detects the corruption
 *   4. Compare effectiveness of different algorithms
 * 
 * Skills Practiced:
 *   - Byte-level data manipulation
 *   - Bitwise operations
 *   - Algorithm implementation
 *   - Data integrity concepts
 * 
 * Security Note:
 *   Checksums detect ACCIDENTAL corruption but are NOT secure against
 *   intentional tampering. For security, use cryptographic hashes.
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t simple_checksum(const uint8_t *data, size_t len) {
    // simple check sum -> add all bytes in data, return the lowest 8 bits

    uint16_t sum = 0;

    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }

    // this will auto calculate the lowest 8 bits if needed
    return (uint8_t)sum;
}

uint8_t xor_checksum(const uint8_t *data, size_t len) {
    uint8_t xor_result = 0;

    size_t i = 0;
    while (i < len) {
        xor_result = xor_result ^ data[i];
        i++;
    }

    return xor_result;
}

uint16_t fletcher16(const uint8_t *data, size_t len) {
    // sum one, sum in order, modulo 255 each time
    // sum of results form each sum in sum one, modulo 255 each time

    uint16_t sum_one = 0;
    uint16_t sum_two = 0;

    size_t i = 0;

    while (i < len) {
        sum_one += data[i];
        sum_two += sum_one;
        
        sum_one = sum_one % 255;
        sum_two = sum_two % 255;

        i++;
    }

    return (sum_two << 8) | sum_one;
}

int main() {
    // Test message
    const char *message = "Mission data: coordinates 12.34, -56.78";
    size_t len = strlen(message);
    
    printf("=== CHECKSUM VALIDATION TEST ===\n\n");
    printf("Original message: \"%s\"\n", message);
    printf("Message length: %zu bytes\n\n", len);
    
    // ========== TEST 1: Compute checksums for original message ==========
    printf("--- Test 1: Original Message Checksums ---\n");
    uint8_t simple_orig = simple_checksum((uint8_t*)message, len);
    uint8_t xor_orig = xor_checksum((uint8_t*)message, len);
    uint16_t fletcher_orig = fletcher16((uint8_t*)message, len);
    
    printf("Simple checksum:   %u\n", simple_orig);
    printf("XOR checksum:      %u\n", xor_orig);
    printf("Fletcher-16:       %u\n\n", fletcher_orig);
    
    // ========== TEST 2: Corrupt one byte and recompute ==========
    printf("--- Test 2: Corrupted Message (byte at index 10 flipped) ---\n");
    
    // Create corrupted copy
    char corrupted[100];
    strcpy(corrupted, message);
    corrupted[10] ^= 0x01;  // Flip lowest bit of byte at index 10
    
    printf("Corrupted message: \"%s\"\n", corrupted);
    printf("Changed character: '%c' -> '%c'\n\n", message[10], corrupted[10]);
    
    uint8_t simple_corrupt = simple_checksum((uint8_t*)corrupted, len);
    uint8_t xor_corrupt = xor_checksum((uint8_t*)corrupted, len);
    uint16_t fletcher_corrupt = fletcher16((uint8_t*)corrupted, len);
    
    printf("Simple checksum:   %u\n", simple_corrupt);
    printf("XOR checksum:      %u\n", xor_corrupt);
    printf("Fletcher-16:       %u\n\n", fletcher_corrupt);
    
    // ========== TEST 3: Verify corruption detection ==========
    printf("--- Test 3: Corruption Detection Results ---\n");
    
    if (simple_orig != simple_corrupt) {
        printf("✓ Simple checksum DETECTED corruption\n");
    } else {
        printf("✗ Simple checksum MISSED corruption\n");
    }
    
    if (xor_orig != xor_corrupt) {
        printf("✓ XOR checksum DETECTED corruption\n");
    } else {
        printf("✗ XOR checksum MISSED corruption\n");
    }
    
    if (fletcher_orig != fletcher_corrupt) {
        printf("✓ Fletcher-16 DETECTED corruption\n");
    } else {
        printf("✗ Fletcher-16 MISSED corruption\n");
    }
    
    printf("\n");
    
    // ========== TEST 4: Test with byte swap (order change) ==========
    printf("--- Test 4: Byte Swap Test ---\n");
    
    char swapped[100];
    strcpy(swapped, message);
    // Swap two bytes
    char temp = swapped[5];
    swapped[5] = swapped[6];
    swapped[6] = temp;
    
    printf("Swapped message: \"%s\"\n", swapped);
    printf("Swapped bytes: positions 5 and 6\n\n");
    
    uint8_t simple_swap = simple_checksum((uint8_t*)swapped, len);
    uint8_t xor_swap = xor_checksum((uint8_t*)swapped, len);
    uint16_t fletcher_swap = fletcher16((uint8_t*)swapped, len);
    
    printf("Original checksums:  Simple=%u, XOR=%u, Fletcher=%u\n", 
           simple_orig, xor_orig, fletcher_orig);
    printf("Swapped checksums:   Simple=%u, XOR=%u, Fletcher=%u\n\n", 
           simple_swap, xor_swap, fletcher_swap);
    
    if (simple_orig != simple_swap) {
        printf("✓ Simple checksum detected byte swap\n");
    } else {
        printf("✗ Simple checksum MISSED byte swap (same sum!)\n");
    }
    
    if (xor_orig != xor_swap) {
        printf("✓ XOR checksum detected byte swap\n");
    } else {
        printf("✗ XOR checksum MISSED byte swap (same XOR!)\n");
    }
    
    if (fletcher_orig != fletcher_swap) {
        printf("✓ Fletcher-16 detected byte swap\n");
    } else {
        printf("✗ Fletcher-16 MISSED byte swap\n");
    }
    
    printf("\n");
    
    // ========== TEST 5: Edge cases ==========
    printf("--- Test 5: Edge Cases ---\n");
    
    // Empty string
    const char *empty = "";
    printf("Empty string checksums:\n");
    printf("  Simple: %u\n", simple_checksum((uint8_t*)empty, 0));
    printf("  XOR: %u\n", xor_checksum((uint8_t*)empty, 0));
    printf("  Fletcher-16: %u\n\n", fletcher16((uint8_t*)empty, 0));
    
    // Single byte
    const char *single = "A";
    printf("Single byte 'A' checksums:\n");
    printf("  Simple: %u\n", simple_checksum((uint8_t*)single, 1));
    printf("  XOR: %u\n", xor_checksum((uint8_t*)single, 1));
    printf("  Fletcher-16: %u\n\n", fletcher16((uint8_t*)single, 1));
    
    // All zeros
    uint8_t zeros[5] = {0, 0, 0, 0, 0};
    printf("Five zero bytes checksums:\n");
    printf("  Simple: %u\n", simple_checksum(zeros, 5));
    printf("  XOR: %u\n", xor_checksum(zeros, 5));
    printf("  Fletcher-16: %u\n\n", fletcher16(zeros, 5));
    
    // All 0xFF
    uint8_t all_ff[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    printf("Five 0xFF bytes checksums:\n");
    printf("  Simple: %u\n", simple_checksum(all_ff, 5));
    printf("  XOR: %u\n", xor_checksum(all_ff, 5));
    printf("  Fletcher-16: %u\n\n", fletcher16(all_ff, 5));
    
    printf("=== ALL TESTS COMPLETE ===\n");
    
    return 0;
}