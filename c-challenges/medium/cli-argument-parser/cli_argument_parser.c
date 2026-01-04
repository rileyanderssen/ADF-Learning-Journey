/*****************************************************************************
 * Challenge 6: Command-Line Argument Parser
 * 
 * Purpose:
 *   Build a robust command-line argument parser to understand how professional
 *   CLI tools work. Defense and systems software often uses command-line
 *   interfaces for configuration, automation, and scripting - parsing arguments
 *   correctly is essential for reliable tool operation.
 * 
 * Objective:
 *   Create a program that parses Unix-style command-line arguments including
 *   short flags, long flags, flags with values, and positional arguments.
 *   Handle invalid input gracefully and provide helpful usage information.
 * 
 * Supported Argument Styles:
 *   Short flags:     -v, -h
 *   Short with value: -o output.txt
 *   Long flags:      --verbose, --help
 *   Long with value: --output=log.txt
 *   Positional args: input.dat (non-flag arguments)
 * 
 * Requirements:
 *   - Parse argc and argv to extract flags and their values
 *   - Support both short (-v) and long (--verbose) flag formats
 *   - Handle flags with values: -o filename or --output=filename
 *   - Store positional arguments (non-flag arguments)
 *   - Detect and report invalid or unknown flags
 *   - Print usage/help information when requested or on error
 *   - Exit gracefully with appropriate error codes
 * 
 * Example Usage:
 *   ./program -v --output=log.txt input.dat
 *   
 *   Should parse as:
 *   - Flag: verbose (enabled)
 *   - Flag: output (value: "log.txt")
 *   - Positional arg: "input.dat"
 * 
 * Test Cases:
 *   1. ./program -h                    (print help and exit)
 *   2. ./program -v input.txt          (verbose flag + positional arg)
 *   3. ./program -o output.txt in.dat  (flag with value + positional)
 *   4. ./program --output=out.txt      (long flag with value)
 *   5. ./program -v --verbose          (both short and long versions)
 *   6. ./program -x                    (invalid flag - error message)
 *   7. ./program                       (no arguments - show usage)
 * 
 * Skills Practiced:
 *   - argc/argv handling
 *   - String parsing and comparison
 *   - User interface design (help messages)
 *   - Input validation
 *   - Error handling and reporting
 * 
 * Implementation Hints:
 *   - argv[0] is the program name
 *   - argv[1] through argv[argc-1] are the arguments
 *   - Use strcmp() to compare strings
 *   - Use strncmp() for prefix matching (e.g., "--output=")
 *   - Use strchr() to find '=' in long flags
 *   - Check if arg starts with '-' to identify flags
 * 
 * Author: Riley Anderssen
 * Date: January 2025
 * Part of: ADF Software Engineer Preparation - C Challenges
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

// verbose is a running mode - means more detailed info if flag is on

int main(int argc, char *argv[]) {
    int help = 0;
    int verbose = 0;
    char *output_file = NULL;
    char *input_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || argc == 1) {
            help = 1;
            break;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            int next_idx = i + 1;

            if (argc <= next_idx) {
                // invalid, need to specify output file
                printf("Error: Output file not specified\n");
                return 0;
            }

            output_file = argv[next_idx];
            i += 1;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        } else if (strncmp(argv[i], "--output=", 9) == 0) {
            output_file = argv[i] + 9;
        } else if (strncmp(argv[i], "--input=", 8) == 0) {
            input_file = argv[i] + 8;
        } else if (argv[i][0] == '-') {
            printf("Error: Unknown flag: %s\n", argv[i]);
        }
    }

    if (help == 1) {
        printf("Usage: ./program [OPTIONS] [FILE]\n\n");
        printf("Options:\n");
        printf("  -v, --verbose       Enable verbose output\n");
        printf("  -h, --help          Show this help message\n");
        printf("  -o FILE             Specify output file\n");
        printf("  --output=FILE       Specify output file (long form)\n\n");
        printf("Example:\n");
        printf("./program -v --output=log.txt input.dat\n");
        return 0;
    }

    if (verbose == 1) {
        printf("VERBOSE: ON\n");
    }

    if (output_file != NULL) {
        printf("Output file: %s\n", output_file);
    }

    if (input_file != NULL) {
        printf("Input file: %s\n", input_file);
    }

    return 0;
}