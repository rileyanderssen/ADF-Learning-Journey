#!/bin/bash

# NOTE: to make bash script an exe, run chmod +x test.sh
# Then run it by: ./test

# Compile the program first
echo "Compiling..."
gcc -Wall -Wextra -o program cli_argument_parser.c
echo ""

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to run a test
run_test() {
    echo -e "${BLUE}Test: $1${NC}"
    echo "Command: ./program $2"
    echo "---"
    ./program $2
    echo ""
    echo "================================"
    echo ""
}

# Run all test cases
echo "=== RUNNING ALL TEST CASES ==="
echo ""

run_test "No arguments (should show usage)" ""

run_test "Help flag (short)" "-h"

run_test "Help flag (long)" "--help"

run_test "Verbose flag" "-v"

run_test "Verbose + input file" "-v input.txt"

run_test "Output file (short)" "-o output.txt"

run_test "Output file (long)" "--output=log.txt"

run_test "Multiple flags" "-v -o output.txt input.dat"

run_test "Long and short flags mixed" "-v --output=log.txt input.dat"

run_test "Both verbose flags" "-v --verbose"

run_test "Invalid flag" "-x"

run_test "Multiple invalid flags" "-x -y -z"

run_test "Output flag without value" "-o"

run_test "Complex combination" "-v --output=result.txt --help input.dat"

echo "=== ALL TESTS COMPLETE ==="