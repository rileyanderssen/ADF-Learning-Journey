/*****************************************************************************
 * Challenge 2: Bitwise Flag Manager
 * 
 * Purpose:
 *   Implement a compact flag management system using bitwise operations
 *   to understand efficient data representation and bit manipulation -
 *   essential skills for embedded systems and low-level programming.
 * 
 * Objective:
 *   Create a system that manages 8 boolean flags using a single byte (uint8_t).
 *   Each bit represents a different system state (e.g., sensor_active, 
 *   emergency_mode, armed, system_ready). This demonstrates how to maximize
 *   memory efficiency in resource-constrained environments like defense
 *   embedded systems.
 * 
 * Function Signatures:
 *   void set_flag(uint8_t *flags, int position)
 *   void clear_flag(uint8_t *flags, int position)
 *   void toggle_flag(uint8_t *flags, int position)
 *   int check_flag(uint8_t flags, int position)
 *   void print_flags(uint8_t flags)
 * 
 * Requirements:
 *   - set_flag: Set the bit at 'position' to 1 (mark flag as active)
 *   - clear_flag: Set the bit at 'position' to 0 (mark flag as inactive)
 *   - toggle_flag: Flip the bit at 'position' (0→1 or 1→0)
 *   - check_flag: Return 1 if bit at 'position' is set, 0 otherwise
 *   - print_flags: Display all 8 bits in binary format (e.g., "10110001")
 *   - Position ranges from 0-7 (rightmost bit is position 0)
 *   - Handle invalid positions gracefully (out of range 0-7)
 * 
 * Test Cases:
 *   1. Set multiple flags and verify with check_flag
 *   2. Clear a set flag and verify it's cleared
 *   3. Toggle a flag twice and verify it returns to original state
 *   4. Print flags after various operations to visualize bit patterns
 *   5. Test boundary positions (0 and 7)
 *   6. Test invalid position handling (negative, >7)
 * 
 * Skills Practiced:
 *   - Bitwise operators: OR (|), AND (&), XOR (^), NOT (~)
 *   - Bit shifting: left shift (<<), right shift (>>)
 *   - Bit masking techniques
 *   - Memory-efficient data structures
 *   - Understanding binary representation
 *   - Embedded systems programming patterns
 * 
 * Real-World Applications:
 *   In defense and embedded systems, memory is often limited. A single byte
 *   can represent 8 separate boolean states instead of using 8 bytes.
 *   Common uses include:
 *   - Hardware register manipulation
 *   - System status indicators
 *   - Permission/capability flags
 *   - Sensor state tracking
 *   - Communication protocol headers
 * 
 * Example Usage:
 *   uint8_t system_flags = 0;  // Initialize: 00000000
 *   set_flag(&system_flags, 0);     // Sensor active:    00000001
 *   set_flag(&system_flags, 3);     // Emergency mode:   00001001
 *   toggle_flag(&system_flags, 7);  // Armed:            10001001
 *   print_flags(system_flags);      // Output: "10001001"
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stdint.h>
#include <stdio.h>

void set_flag(uint8_t *flags, int position) {
    // shift the 1 to the position to flip
    uint8_t mask = 1 << position;


    *flags = *flags | mask;
}

void clear_flag(uint8_t *flags, int position) {
    // shift one to position to clear
    uint8_t mask = 1 << position;

    // set the bit
    *flags = *flags | mask;

    // invert the mask
    mask = ~mask;

    // and with flags (clears everything that is not 1 on both sides i.e. clearing the target bit as it will be 0 in the inverted mask)
    *flags = *flags & mask;

}

void toggle_flag(uint8_t *flags, int position) {
    // to toggle, use XOR (^) with a 1 at the position to toggle

    uint8_t mask = 1 << position;

    *flags = *flags ^ mask;
}

int check_flag(uint8_t flags, int position) {
    // check using AND ... with 1, 0 would clear the current ones

    uint8_t mask = 1 << position;

    uint8_t result = flags & mask;

    if (result != 0) {
        return 1;
    }

    return 0;
}

void print_flags(uint8_t flags) {
    // loop from MSB (left) to LSB (right) printing each binary value
    int i = 7;
    
    printf("Binary representation: ");

    while (i > -1) {
        printf("%d", check_flag(flags, i));
        i--;
    }

    printf("\n");
}

int main() {
    printf("=== Testing Bitwise Flag Manager ===\n\n");
    
    // Test Case 1: Set multiple flags and verify with check_flag
    printf("Test 1: Set multiple flags and verify\n");
    uint8_t flags = 0b00000000;
    printf("  Initial flags: ");
    print_flags(flags);
    
    set_flag(&flags, 0);
    set_flag(&flags, 3);
    set_flag(&flags, 7);
    
    printf("  After setting bits 0, 3, 7: ");
    print_flags(flags);
    
    printf("  Check bit 0: %d (expected: 1)\n", check_flag(flags, 0));
    printf("  Check bit 3: %d (expected: 1)\n", check_flag(flags, 3));
    printf("  Check bit 7: %d (expected: 1)\n", check_flag(flags, 7));
    printf("  Check bit 1: %d (expected: 0)\n", check_flag(flags, 1));
    printf("  Status: %s\n\n", 
           (check_flag(flags, 0) && check_flag(flags, 3) && 
            check_flag(flags, 7) && !check_flag(flags, 1)) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 2: Clear a set flag and verify it's cleared
    printf("Test 2: Clear a set flag\n");
    flags = 0b11111111;  // All bits set
    printf("  Initial flags (all set): ");
    print_flags(flags);
    
    clear_flag(&flags, 3);
    clear_flag(&flags, 5);
    
    printf("  After clearing bits 3, 5: ");
    print_flags(flags);
    
    printf("  Check bit 3: %d (expected: 0)\n", check_flag(flags, 3));
    printf("  Check bit 5: %d (expected: 0)\n", check_flag(flags, 5));
    printf("  Check bit 2: %d (expected: 1)\n", check_flag(flags, 2));
    printf("  Status: %s\n\n", 
           (!check_flag(flags, 3) && !check_flag(flags, 5) && 
            check_flag(flags, 2)) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 3: Toggle a flag twice and verify it returns to original state
    printf("Test 3: Toggle flag twice (returns to original)\n");
    flags = 0b00001010;  // Bits 1 and 3 set
    printf("  Initial flags: ");
    print_flags(flags);
    
    printf("  Toggle bit 5 (0→1): ");
    toggle_flag(&flags, 5);
    print_flags(flags);
    printf("    Check bit 5: %d (expected: 1)\n", check_flag(flags, 5));
    
    printf("  Toggle bit 5 again (1→0): ");
    toggle_flag(&flags, 5);
    print_flags(flags);
    printf("    Check bit 5: %d (expected: 0)\n", check_flag(flags, 5));
    
    printf("  Toggle bit 3 (1→0): ");
    toggle_flag(&flags, 3);
    print_flags(flags);
    printf("    Check bit 3: %d (expected: 0)\n", check_flag(flags, 3));
    
    printf("  Toggle bit 3 again (0→1): ");
    toggle_flag(&flags, 3);
    print_flags(flags);
    printf("    Check bit 3: %d (expected: 1)\n", check_flag(flags, 3));
    
    printf("  Status: %s\n\n", (flags == 0b00001010) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 4: Print flags after various operations to visualize bit patterns
    printf("Test 4: Visualize bit patterns after operations\n");
    flags = 0b00000000;
    printf("  Start with all clear: ");
    print_flags(flags);
    
    set_flag(&flags, 0);
    printf("  Set bit 0:            ");
    print_flags(flags);
    
    set_flag(&flags, 2);
    printf("  Set bit 2:            ");
    print_flags(flags);
    
    set_flag(&flags, 4);
    printf("  Set bit 4:            ");
    print_flags(flags);
    
    toggle_flag(&flags, 2);
    printf("  Toggle bit 2:         ");
    print_flags(flags);
    
    clear_flag(&flags, 0);
    printf("  Clear bit 0:          ");
    print_flags(flags);
    
    printf("  Expected final: 00010000\n");
    printf("  Status: %s\n\n", (flags == 0b00010000) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 5: Test boundary positions (0 and 7)
    printf("Test 5: Boundary positions (0 and 7)\n");
    flags = 0b00000000;
    
    printf("  Set bit 0 (rightmost): ");
    set_flag(&flags, 0);
    print_flags(flags);
    printf("    Check bit 0: %d (expected: 1)\n", check_flag(flags, 0));
    
    printf("  Set bit 7 (leftmost):  ");
    set_flag(&flags, 7);
    print_flags(flags);
    printf("    Check bit 7: %d (expected: 1)\n", check_flag(flags, 7));
    
    printf("  Clear bit 0:           ");
    clear_flag(&flags, 0);
    print_flags(flags);
    
    printf("  Clear bit 7:           ");
    clear_flag(&flags, 7);
    print_flags(flags);
    
    printf("  Status: %s\n\n", (flags == 0b00000000) ? "PASS ✓" : "FAIL ✗");
    
    // Test Case 6: Test invalid position handling (negative, >7)
    printf("Test 6: Invalid position handling\n");
    flags = 0b01010101;
    printf("  Initial flags: ");
    print_flags(flags);
    
    printf("  Attempting to set bit -1 (invalid)...\n");
    set_flag(&flags, -1);
    printf("  Flags after: ");
    print_flags(flags);
    printf("  Flags unchanged: %s\n", (flags == 0b01010101) ? "YES ✓" : "NO ✗");
    
    printf("  Attempting to set bit 8 (invalid)...\n");
    set_flag(&flags, 8);
    printf("  Flags after: ");
    print_flags(flags);
    printf("  Flags unchanged: %s\n", (flags == 0b01010101) ? "YES ✓" : "NO ✗");
    
    printf("  Attempting to check bit 10 (invalid)...\n");
    int result = check_flag(flags, 10);
    printf("  Result: %d (expected: 0 or handled gracefully)\n", result);
    
    printf("  Status: %s\n\n", (flags == 0b01010101) ? "PASS ✓" : "FAIL ✗");
    
    printf("=== All tests complete ===\n");
    
    return 0;
}