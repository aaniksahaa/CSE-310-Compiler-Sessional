#!/bin/bash

# Set debugging variable
DEBUG=1

# Debug file prefix
DEBUG_FILE_PREFIX=""

# Declare variables for file names
YACC_FILE="2005001.y"
LEX_FILE="2005001.l"
INPUT_FILE="input.c"

# Declare variables for intermediate files
YACC_OUTPUT="y.tab"
LEX_OUTPUT="lex.yy"
DEBUG_FILE="${DEBUG_FILE_PREFIX}debug.txt"
EXECUTABLE_NAME="a"

# Run yacc to generate parser files
yacc -d -y "$YACC_FILE"
echo 'Generated the parser C file as well as the header file'

# Compile the parser object file
g++ -w -c -o y.o "${YACC_OUTPUT}.c"
echo 'Generated the parser object file'

# Run flex to generate scanner files
flex "$LEX_FILE"
echo 'Generated the scanner C file'

# Compile the scanner object file
g++ -w -c -o l.o "${LEX_OUTPUT}.c"
# if the above command doesn't work, try g++ -fpermissive -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'

# Link the parser and scanner object files to create the executable
g++ y.o l.o -lfl -o "$EXECUTABLE_NAME"
echo 'All ready, running'

# Run the executable with the specified input file
./"$EXECUTABLE_NAME" "$INPUT_FILE"

# Check if debugging is enabled before deleting the debug file
if [ "$DEBUG" -eq 1 ]; then
    echo 'Debugging enabled, keeping the debug file'
else
    rm -f "${DEBUG_FILE}"
    echo 'Debug file deleted as debug mode is off'
fi

# Clean up intermediate files (excluding debug file) and executable
rm -f "${LEX_OUTPUT}.c" "${YACC_OUTPUT}.c" "${YACC_OUTPUT}.h" l.o y.o "$EXECUTABLE_NAME"
echo 'Cleaned up intermediate files and executable'
