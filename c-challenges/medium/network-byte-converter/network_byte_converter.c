/*****************************************************************************
 * Challenge 9: Network Byte Order Converter
 * 
 * Purpose:
 *   Understand endianness and network byte order conversion - critical for
 *   network protocols and cross-platform communication in defense systems.
 *   Military systems exchange data between different architectures (x86, ARM,
 *   etc.) and must handle byte order correctly to ensure data integrity.
 * 
 * Objective:
 *   Implement serialization and deserialization functions that convert a
 *   message struct to/from network byte order (big-endian). This ensures
 *   data can be transmitted over a network and correctly interpreted by
 *   systems with different endianness.
 * 
 * Data Structure:
 *   typedef struct {
 *       uint16_t msg_type;        // Message type identifier (2 bytes)
 *       uint32_t timestamp;       // Unix timestamp (4 bytes)
 *       uint16_t payload_length;  // Length of payload (2 bytes)
 *       uint8_t payload[256];     // Message payload (256 bytes max)
 *   } NetworkMessage;
 * 
 * Requirements:
 *   - serialize_message(): Convert struct to network byte order in buffer
 *   - deserialize_message(): Convert buffer from network byte order to struct
 *   - Use htons(), htonl(), ntohs(), ntohl() for byte order conversion
 *   - Handle multi-byte integers (uint16_t, uint32_t) correctly
 *   - Payload bytes don't need conversion (already byte-sized)
 * 
 * Key Functions to Implement:
 *   void serialize_message(NetworkMessage *msg, uint8_t *buffer)
 *   void deserialize_message(uint8_t *buffer, NetworkMessage *msg)
 * 
 * Byte Order Conversion Functions:
 *   - htons(x)  - Host to Network Short (uint16_t)
 *   - htonl(x)  - Host to Network Long (uint32_t)
 *   - ntohs(x)  - Network to Host Short (uint16_t)
 *   - ntohl(x)  - Network to Host Long (uint32_t)
 *   
 *   #include <arpa/inet.h>  // For these functions
 * 
 * Endianness Explained:
 *   Big-Endian (Network Byte Order):
 *     Value 0x12345678 stored as: [12][34][56][78]
 *     Most significant byte first
 *   
 *   Little-Endian (Most x86/x64 systems):
 *     Value 0x12345678 stored as: [78][56][34][12]
 *     Least significant byte first
 * 
 * Test Cases:
 *   1. Create message, serialize, deserialize, verify values match
 *   2. Test with different msg_type values (0, 1, 65535)
 *   3. Test with different timestamps
 *   4. Test with various payload lengths (0, 100, 256)
 *   5. Test with different payload contents
 *   6. Verify serialized buffer has correct byte order
 * 
 * Skills Practiced:
 *   - Understanding endianness
 *   - Network byte order conversion
 *   - Binary serialization
 *   - Struct packing/unpacking
 *   - Cross-platform data compatibility
 * 
 * Buffer Layout After Serialization:
 *   Bytes 0-1:   msg_type (big-endian uint16_t)
 *   Bytes 2-5:   timestamp (big-endian uint32_t)
 *   Bytes 6-7:   payload_length (big-endian uint16_t)
 *   Bytes 8-263: payload (256 bytes, no conversion needed)
 * 
 * Example:
 *   NetworkMessage msg = {
 *       .msg_type = 1,
 *       .timestamp = 1704067200,
 *       .payload_length = 5,
 *       .payload = "Hello"
 *   };
 *   
 *   uint8_t buffer[264];
 *   serialize_message(&msg, buffer);
 *   // buffer now contains big-endian representation
 *   
 *   NetworkMessage received;
 *   deserialize_message(buffer, &received);
 *   // received now has correct values on any architecture
 * 
 * Why This Matters:
 *   - Network protocols (TCP/IP, UDP) use big-endian
 *   - Data exchange between different CPU architectures
 *   - File formats that must work cross-platform
 *   - Defense systems communicate between diverse hardware
 *   - Radar data, telemetry, command messages all need this
 * 
 * Common Mistakes to Avoid:
 *   - Forgetting to convert multi-byte integers
 *   - Converting payload bytes (they're already byte-sized!)
 *   - Using wrong conversion function (short vs long)
 *   - Not handling payload_length correctly
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/


#include <stdlib.h>
#include <arpa/inet.h>  
#include <string.h>     
#include <stdio.h>

typedef struct {
    uint16_t msg_type; // message type identifier .. 2 bytes
    uint32_t timestamp; // unix timestamp .. 4 bytes
    uint16_t payload_length; // length of payload (2 bytes)
    uint8_t payload[256]; // message payload .. 256 bytes max
} NetworkMessage;

// Note -> the htons htonl etc, do not change the size, they change the byte order
// do some further research on little-endian and big-endian

void serialize_message(NetworkMessage *msg, uint8_t *buffer) {
    // copy in msg type
    uint16_t network_msg_type = htons(msg->msg_type);
    memcpy(buffer, &network_msg_type, 2);

    // copy in time stamp
    uint32_t timestamp = htonl(msg->timestamp);
    memcpy(buffer + 2, &timestamp, 4);

    // copy in the payload length
    uint16_t payload_length = htons(msg->payload_length);
    memcpy(buffer + 6, &payload_length, 2);

    // copy in payload
    memcpy(buffer + 8, msg->payload, 256);
}

void deserialize_message(uint8_t *buffer, NetworkMessage *msg) {
    // essentially the same thing as serialize but in reverse

    uint16_t network_msg_type;
    memcpy(&network_msg_type, buffer, 2);
    msg->msg_type = ntohs(network_msg_type);

    uint32_t timestamp;
    memcpy(&timestamp, buffer + 2, 4);
    msg->timestamp = ntohl(timestamp);

    uint16_t payload_length;
    memcpy(&payload_length, buffer + 6, 2);
    msg->payload_length = ntohs(payload_length);

    memcpy(msg->payload, buffer + 8, 256);
}   

int main() {
    printf("=== NETWORK BYTE ORDER CONVERTER TEST SUITE ===\n\n");
    
    // ========== TEST 1: Basic serialize and deserialize ==========
    printf("--- Test 1: Basic Serialize/Deserialize ---\n");
    
    NetworkMessage original = {
        .msg_type = 42,
        .timestamp = 1704067200,
        .payload_length = 11,
    };
    strcpy((char*)original.payload, "Hello World");
    
    uint8_t buffer[264];
    serialize_message(&original, buffer);
    
    NetworkMessage received;
    deserialize_message(buffer, &received);
    
    printf("Original:  msg_type=%u, timestamp=%u, payload_length=%u, payload=%s\n",
           original.msg_type, original.timestamp, original.payload_length, original.payload);
    printf("Received:  msg_type=%u, timestamp=%u, payload_length=%u, payload=%s\n",
           received.msg_type, received.timestamp, received.payload_length, received.payload);
    
    if (original.msg_type == received.msg_type &&
        original.timestamp == received.timestamp &&
        original.payload_length == received.payload_length &&
        strcmp((char*)original.payload, (char*)received.payload) == 0) {
        printf("✓ Test 1 PASSED\n");
    } else {
        printf("✗ Test 1 FAILED\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 2: Edge case values ==========
    printf("--- Test 2: Edge Case Values ---\n");
    
    NetworkMessage edge_case = {
        .msg_type = 0,
        .timestamp = 0,
        .payload_length = 0,
    };
    memset(edge_case.payload, 0, 256);
    
    serialize_message(&edge_case, buffer);
    deserialize_message(buffer, &received);
    
    printf("Testing all zeros:\n");
    printf("  msg_type: %u (expected 0)\n", received.msg_type);
    printf("  timestamp: %u (expected 0)\n", received.timestamp);
    printf("  payload_length: %u (expected 0)\n", received.payload_length);
    
    if (received.msg_type == 0 && received.timestamp == 0 && received.payload_length == 0) {
        printf("✓ Test 2a PASSED (zeros)\n");
    } else {
        printf("✗ Test 2a FAILED (zeros)\n");
    }
    
    // Test maximum values
    edge_case.msg_type = 65535;      // Max uint16_t
    edge_case.timestamp = 4294967295; // Max uint32_t
    edge_case.payload_length = 256;
    
    serialize_message(&edge_case, buffer);
    deserialize_message(buffer, &received);
    
    printf("\nTesting maximum values:\n");
    printf("  msg_type: %u (expected 65535)\n", received.msg_type);
    printf("  timestamp: %u (expected 4294967295)\n", received.timestamp);
    printf("  payload_length: %u (expected 256)\n", received.payload_length);
    
    if (received.msg_type == 65535 && 
        received.timestamp == 4294967295 && 
        received.payload_length == 256) {
        printf("✓ Test 2b PASSED (max values)\n");
    } else {
        printf("✗ Test 2b FAILED (max values)\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 3: Different payload sizes ==========
    printf("--- Test 3: Various Payload Sizes ---\n");
    
    // Empty payload
    NetworkMessage msg_empty = {
        .msg_type = 1,
        .timestamp = 1000,
        .payload_length = 0,
    };
    memset(msg_empty.payload, 0, 256);
    
    serialize_message(&msg_empty, buffer);
    deserialize_message(buffer, &received);
    
    printf("Empty payload (length=0): ");
    if (received.payload_length == 0) {
        printf("✓ PASSED\n");
    } else {
        printf("✗ FAILED\n");
    }
    
    // Medium payload
    NetworkMessage msg_medium = {
        .msg_type = 2,
        .timestamp = 2000,
        .payload_length = 100,
    };
    for (int i = 0; i < 100; i++) {
        msg_medium.payload[i] = i % 256;
    }
    
    serialize_message(&msg_medium, buffer);
    deserialize_message(buffer, &received);
    
    printf("Medium payload (length=100): ");
    int medium_match = 1;
    for (int i = 0; i < 100; i++) {
        if (received.payload[i] != i % 256) {
            medium_match = 0;
            break;
        }
    }
    if (received.payload_length == 100 && medium_match) {
        printf("✓ PASSED\n");
    } else {
        printf("✗ FAILED\n");
    }
    
    // Full payload
    NetworkMessage msg_full = {
        .msg_type = 3,
        .timestamp = 3000,
        .payload_length = 256,
    };
    for (int i = 0; i < 256; i++) {
        msg_full.payload[i] = 255 - i;
    }
    
    serialize_message(&msg_full, buffer);
    deserialize_message(buffer, &received);
    
    printf("Full payload (length=256): ");
    int full_match = 1;
    for (int i = 0; i < 256; i++) {
        if (received.payload[i] != 255 - i) {
            full_match = 0;
            break;
        }
    }
    if (received.payload_length == 256 && full_match) {
        printf("✓ PASSED\n");
    } else {
        printf("✗ FAILED\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 4: Byte order verification ==========
    printf("--- Test 4: Verify Network Byte Order ---\n");
    
    NetworkMessage msg_verify = {
        .msg_type = 0x1234,
        .timestamp = 0x12345678,
        .payload_length = 0xABCD,
    };
    memset(msg_verify.payload, 0, 256);
    
    serialize_message(&msg_verify, buffer);
    
    printf("Serialized buffer (first 8 bytes in hex):\n");
    printf("  ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    
    printf("\nExpected on little-endian system (network = big-endian):\n");
    printf("  Bytes 0-1 (msg_type):       12 34 (big-endian)\n");
    printf("  Bytes 2-5 (timestamp):      12 34 56 78 (big-endian)\n");
    printf("  Bytes 6-7 (payload_length): AB CD (big-endian)\n");
    
    int byte_order_correct = 1;
    if (buffer[0] != 0x12 || buffer[1] != 0x34) {
        printf("✗ msg_type byte order incorrect\n");
        byte_order_correct = 0;
    }
    if (buffer[2] != 0x12 || buffer[3] != 0x34 || 
        buffer[4] != 0x56 || buffer[5] != 0x78) {
        printf("✗ timestamp byte order incorrect\n");
        byte_order_correct = 0;
    }
    if (buffer[6] != 0xAB || buffer[7] != 0xCD) {
        printf("✗ payload_length byte order incorrect\n");
        byte_order_correct = 0;
    }
    
    if (byte_order_correct) {
        printf("✓ Test 4 PASSED (correct network byte order)\n");
    } else {
        printf("✗ Test 4 FAILED (incorrect byte order)\n");
    }
    
    printf("\n================================\n\n");
    
    // ========== TEST 5: Multiple serialize/deserialize cycles ==========
    printf("--- Test 5: Multiple Cycles ---\n");
    
    NetworkMessage cycle_msg = {
        .msg_type = 999,
        .timestamp = 1234567890,
        .payload_length = 50,
    };
    strcpy((char*)cycle_msg.payload, "Testing multiple serialize/deserialize cycles");
    
    int cycles_passed = 1;
    for (int i = 0; i < 10; i++) {
        serialize_message(&cycle_msg, buffer);
        NetworkMessage temp;
        deserialize_message(buffer, &temp);
        
        if (temp.msg_type != cycle_msg.msg_type ||
            temp.timestamp != cycle_msg.timestamp ||
            temp.payload_length != cycle_msg.payload_length) {
            cycles_passed = 0;
            printf("✗ Cycle %d failed\n", i + 1);
            break;
        }
        
        cycle_msg = temp; // Use deserialized as input for next cycle
    }
    
    if (cycles_passed) {
        printf("✓ Test 5 PASSED (10 cycles successful)\n");
    } else {
        printf("✗ Test 5 FAILED\n");
    }
    
    printf("\n================================\n\n");
    
    printf("=== ALL TESTS COMPLETE ===\n");
    
    return 0;
}