/*****************************************************************************
 * Challenge 4: Binary File I/O - Sensor Data Logger
 * 
 * Purpose:
 *   Implement binary file input/output operations to understand low-level
 *   data persistence and serialization - critical skills for embedded systems,
 *   data logging, and defense applications where efficient storage and
 *   retrieval of structured data is essential.
 * 
 * Objective:
 *   Create a sensor data logging system that writes and reads SensorReading
 *   structs to/from binary files. This demonstrates how to persist structured
 *   data efficiently without the overhead of text-based formats like JSON or
 *   CSV. Binary files are compact, fast to read/write, and preserve exact
 *   data representations - crucial for high-frequency sensor systems.
 * 
 * Data Structure:
 *   typedef struct {
 *       uint32_t timestamp;    // Unix timestamp or milliseconds since start
 *       float temperature;     // Temperature in Celsius
 *       float pressure;        // Pressure in kPa
 *       uint16_t sensor_id;    // Unique sensor identifier
 *   } SensorReading;
 * 
 * Function Signatures:
 *   int write_reading(FILE *fp, SensorReading *reading)
 *   int read_reading(FILE *fp, SensorReading *reading)
 *   void read_all_readings(const char *filename)
 *   void generate_sample_data(const char *filename, int count)
 * 
 * Requirements:
 *   - write_reading: Write a single SensorReading struct to binary file
 *                    Return 1 on success, 0 on failure
 *                    Use fwrite() to write raw bytes
 *   - read_reading: Read a single SensorReading struct from binary file
 *                   Return 1 on success, 0 on failure or EOF
 *                   Use fread() to read raw bytes
 *   - read_all_readings: Open file, read all readings, print them formatted
 *                        Handle file opening errors gracefully
 *   - generate_sample_data: Create 100 random sensor readings and save to file
 *                           Use realistic ranges for sensor values
 * 
 * File Operations:
 *   - Open for writing: fopen(filename, "wb")  // "wb" = write binary
 *   - Open for reading: fopen(filename, "rb")  // "rb" = read binary
 *   - Always check if fopen() returns NULL (file open failed)
 *   - Always fclose() when done to flush buffers and free resources
 * 
 * Test Cases:
 *   1. Write a single reading to file and verify file created
 *   2. Write multiple readings and check file size is correct
 *   3. Read back written data and verify values match
 *   4. Generate 100 random readings, save, and read back
 *   5. Test reading from non-existent file (should handle gracefully)
 *   6. Test writing to read-only location (should handle errors)
 *   7. Verify binary file format (size = num_readings * sizeof(SensorReading))
 * 
 * Skills Practiced:
 *   - Binary file I/O: fopen(), fread(), fwrite(), fclose()
 *   - File modes: "rb" (read binary), "wb" (write binary)
 *   - Struct serialization (writing raw struct bytes)
 *   - Error handling with file operations
 *   - EOF detection and handling
 *   - Random data generation for testing
 *   - Data validation and verification
 * 
 * Binary vs Text Files:
 *   TEXT FILE (CSV):
 *   "1234567890,23.5,101.3,42\n"  (29 bytes per reading)
 *   - Human readable
 *   - Larger file size
 *   - Slower parsing
 *   - Precision loss possible
 *   
 *   BINARY FILE:
 *   [raw bytes: 4+4+4+2]  (14 bytes per reading)
 *   - Compact (52% smaller!)
 *   - Fast read/write (no parsing)
 *   - Exact precision
 *   - Not human-readable
 * 
 * Real-World Applications:
 *   In defense and embedded systems, binary files are used for:
 *   - Flight data recorders (black boxes)
 *   - Telemetry data from satellites
 *   - High-frequency sensor logging (radar, sonar)
 *   - Firmware updates and configuration storage
 *   - Video/audio recording systems
 *   - Network packet captures
 *   
 *   Benefits in resource-constrained environments:
 *   - Minimal storage space (no text overhead)
 *   - Fast I/O (direct memory-to-disk writes)
 *   - No parsing/formatting overhead
 *   - Deterministic read/write times
 * 
 * Example Usage:
 *   // Write readings to file
 *   FILE *fp = fopen("sensor_log.bin", "wb");
 *   SensorReading reading = {1234567890, 23.5, 101.3, 42};
 *   write_reading(fp, &reading);
 *   fclose(fp);
 *   
 *   // Read all readings from file
 *   read_all_readings("sensor_log.bin");
 *   
 *   // Generate test data
 *   generate_sample_data("test_data.bin", 100);
 * 
 * Important Notes:
 *   - Binary files are NOT portable across different architectures
 *     (endianness, struct padding may differ)
 *   - For production systems, consider using standardized formats
 *     (e.g., Protocol Buffers, MessagePack) for cross-platform compatibility
 *   - Always check return values of fread/fwrite/fopen
 *   - Remember to fclose() files to prevent resource leaks
 *   - Use sizeof(SensorReading) to ensure correct byte counts
 * 
 * File Structure:
 *   sensor_log.bin:
 *   ┌─────────────────────────┐
 *   │ SensorReading #1        │  14 bytes
 *   │ (timestamp, temp, etc.) │
 *   ├─────────────────────────┤
 *   │ SensorReading #2        │  14 bytes
 *   ├─────────────────────────┤
 *   │ SensorReading #3        │  14 bytes
 *   ├─────────────────────────┤
 *   │ ...                     │
 *   └─────────────────────────┘
 *   Total size = count * sizeof(SensorReading)
 * 
 * Security Considerations:
 *   - Validate file paths to prevent directory traversal attacks
 *   - Check file permissions before writing
 *   - Validate data ranges when reading (sensor values should be realistic)
 *   - Handle corrupted files gracefully
 *   - Sanitize filenames from user input
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>    

typedef struct {
    uint32_t timestamp;
    float temperature;
    float pressure;
    uint16_t sensor_id;
} SensorReading;

int write_reading(FILE *fp, SensorReading *reading) {
    size_t written = fwrite(reading, sizeof(SensorReading), 1, fp);

    if (written != -1) {
        // success
        return 1;
    }

    return 0;
}

// this works by reading the next item from the bin file
// the current item is persisted across calls
// this is because fp holds a pointer to the current file position
int read_reading(FILE *fp, SensorReading *reading) {
    if (fp == NULL || reading == NULL) {
        return 0;
    }

    size_t items_read = fread(reading, sizeof(SensorReading), 1, fp);

    if (items_read != 1) {
        return 0;
    }

    return 1;
}

void read_all_readings(const char *filename) {
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("Nothing to read all from");
        return;
    }

    SensorReading r;
    int count = 0;

    printf("Reading sensor data from '%s':\n", filename);
    printf("─────────────────────────────────────────────────────────\n");
    
    while (read_reading(fp, &r) == 1) {
        printf("[%3d] Time: %10u, Temp: %6.1f°C, Press: %6.1f kPa, Sensor: %3u\n",
               count, r.timestamp, r.temperature,
               r.pressure, r.sensor_id);
        count++;
    }
    
    printf("─────────────────────────────────────────────────────────\n");
    printf("Total readings: %d\n", count);

    fclose(fp);
}

void generate_sample_data(const char *filename, int count) {
    srand(time(NULL));

    FILE *fp = fopen(filename, "wb"); // wb means write binary
    if (fp == NULL) {
        printf("Error: Could not open file '%s' for writing\n", filename);
        return;
    }
    
    printf("Generating %d sensor readings...\n", count);

    for (int i = 0; i < count; i++) {
        SensorReading reading;
    
        reading.timestamp = (uint32_t)(time(NULL) + i);  
        reading.temperature = 15.0f + (rand() % 300) / 10.0f;
        reading.pressure = 95.0f + (rand() % 200) / 10.0f;   
        reading.sensor_id = (uint16_t)(rand() % 100);        
        
        if (write_reading(fp, &reading) == 0) {
            printf("Error: Failed to write reading %d\n", i);
            fclose(fp);
            return;
        }
        
        if ((i + 1) % 20 == 0) {
            printf("  Written %d/%d readings...\n", i + 1, count);
        }
    }

    fclose(fp);
    
    printf("Successfully wrote %d readings to '%s'\n", count, filename);
    printf("File size: %zu bytes\n", count * sizeof(SensorReading));
}

int main() {
    printf("=== Testing Binary File I/O - Sensor Data Logger ===\n\n");
    
    // Test Case 1: Write a single reading to file and verify file created
    printf("Test 1: Write single reading and verify file created\n");
    FILE *fp1 = fopen("test_single.bin", "wb");
    if (fp1 == NULL) {
        printf("  FAIL ✗ - Could not create file\n\n");
    } else {
        SensorReading test_reading = {1234567890, 23.5f, 101.3f, 42};
        int result = write_reading(fp1, &test_reading);
        fclose(fp1);
        
        // Check if file exists
        FILE *check = fopen("test_single.bin", "rb");
        if (check != NULL) {
            fclose(check);
            printf("  File created: YES ✓\n");
            printf("  Write result: %d (expected: 1)\n", result);
            printf("  Status: %s\n\n", (result == 1) ? "PASS ✓" : "FAIL ✗");
        } else {
            printf("  FAIL ✗ - File not found\n\n");
        }
    }
    
    // Test Case 2: Write multiple readings and check file size is correct
    printf("Test 2: Write multiple readings and verify file size\n");
    FILE *fp2 = fopen("test_multiple.bin", "wb");
    if (fp2 == NULL) {
        printf("  FAIL ✗ - Could not create file\n\n");
    } else {
        int num_readings = 5;
        for (int i = 0; i < num_readings; i++) {
            SensorReading r = {
                (uint32_t)(1000000 + i),
                20.0f + i,
                100.0f + i,
                (uint16_t)i
            };
            write_reading(fp2, &r);
        }
        fclose(fp2);
        
        // Check file size
        FILE *check = fopen("test_multiple.bin", "rb");
        if (check != NULL) {
            fseek(check, 0, SEEK_END);
            long file_size = ftell(check);
            fclose(check);
            
            size_t expected_size = num_readings * sizeof(SensorReading);
            printf("  Readings written: %d\n", num_readings);
            printf("  File size: %ld bytes\n", file_size);
            printf("  Expected size: %zu bytes\n", expected_size);
            printf("  Status: %s\n\n", (file_size == expected_size) ? "PASS ✓" : "FAIL ✗");
        } else {
            printf("  FAIL ✗ - Could not verify file\n\n");
        }
    }
    
    // Test Case 3: Read back written data and verify values match
    printf("Test 3: Read back data and verify values\n");
    FILE *fp3_write = fopen("test_verify.bin", "wb");
    if (fp3_write == NULL) {
        printf("  FAIL ✗ - Could not create file\n\n");
    } else {
        // Write known data
        SensorReading original = {1234567890, 25.7f, 102.4f, 99};
        write_reading(fp3_write, &original);
        fclose(fp3_write);
        
        // Read it back
        FILE *fp3_read = fopen("test_verify.bin", "rb");
        if (fp3_read == NULL) {
            printf("  FAIL ✗ - Could not open for reading\n\n");
        } else {
            SensorReading read_back;
            int result = read_reading(fp3_read, &read_back);
            fclose(fp3_read);
            
            printf("  Original:  Time=%u, Temp=%.1f, Press=%.1f, ID=%u\n",
                   original.timestamp, original.temperature, 
                   original.pressure, original.sensor_id);
            printf("  Read back: Time=%u, Temp=%.1f, Press=%.1f, ID=%u\n",
                   read_back.timestamp, read_back.temperature,
                   read_back.pressure, read_back.sensor_id);
            
            int match = (original.timestamp == read_back.timestamp &&
                        original.temperature == read_back.temperature &&
                        original.pressure == read_back.pressure &&
                        original.sensor_id == read_back.sensor_id);
            
            printf("  Status: %s\n\n", match ? "PASS ✓" : "FAIL ✗");
        }
    }
    
    // Test Case 4: Generate 100 random readings, save, and read back
    printf("Test 4: Generate 100 random readings\n");
    generate_sample_data("test_100_readings.bin", 100);
    
    // Verify by reading them back
    FILE *fp4 = fopen("test_100_readings.bin", "rb");
    if (fp4 == NULL) {
        printf("  FAIL ✗ - Could not open generated file\n\n");
    } else {
        SensorReading r;
        int count = 0;
        while (read_reading(fp4, &r) == 1) {
            count++;
        }
        fclose(fp4);
        
        printf("  Readings read back: %d\n", count);
        printf("  Expected: 100\n");
        printf("  Status: %s\n\n", (count == 100) ? "PASS ✓" : "FAIL ✗");
    }
    
    // Test Case 5: Test reading from non-existent file (should handle gracefully)
    printf("Test 5: Read from non-existent file\n");
    read_all_readings("this_file_does_not_exist.bin");
    printf("  Status: PASS ✓ (handled gracefully, no crash)\n\n");
    
    // Test Case 6: Test writing to read-only location (error handling)
    printf("Test 6: Write to invalid location\n");
    FILE *fp6 = fopen("/invalid/path/test.bin", "wb");
    if (fp6 == NULL) {
        printf("  Cannot open invalid path: EXPECTED ✓\n");
        printf("  Status: PASS ✓ (error handled gracefully)\n\n");
    } else {
        printf("  Unexpectedly opened invalid path\n");
        fclose(fp6);
        printf("  Status: FAIL ✗\n\n");
    }
    
    // Test Case 7: Verify binary file format
    printf("Test 7: Verify binary file format\n");
    printf("  sizeof(SensorReading): %zu bytes\n", sizeof(SensorReading));
    
    int test_count = 7;
    generate_sample_data("test_format.bin", test_count);
    
    FILE *fp7 = fopen("test_format.bin", "rb");
    if (fp7 == NULL) {
        printf("  FAIL ✗ - Could not open file\n\n");
    } else {
        fseek(fp7, 0, SEEK_END);
        long actual_size = ftell(fp7);
        fclose(fp7);
        
        size_t expected_size = test_count * sizeof(SensorReading);
        
        printf("  Number of readings: %d\n", test_count);
        printf("  Expected file size: %zu bytes (%d × %zu)\n", 
               expected_size, test_count, sizeof(SensorReading));
        printf("  Actual file size: %ld bytes\n", actual_size);
        printf("  Formula correct: %s\n", 
               (actual_size == expected_size) ? "YES ✓" : "NO ✗");
        printf("  Status: %s\n\n", 
               (actual_size == expected_size) ? "PASS ✓" : "FAIL ✗");
    }
    
    // Bonus: Display sample data
    printf("=== Bonus: Display Sample Data ===\n");
    generate_sample_data("display_sample.bin", 10);
    read_all_readings("display_sample.bin");
    
    printf("\n=== All tests complete ===\n");
    printf("\nCleanup: Remove test files with:\n");
    printf("  rm test_*.bin display_sample.bin\n");
    
    // to clean up files: rm test_*.bin display_sample.bin

    return 0;
}