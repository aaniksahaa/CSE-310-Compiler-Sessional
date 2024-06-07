#!/bin/bash

# Navigate to the current directory
cd "$(dirname "$0")"

# Remove all .txt files
find . -maxdepth 1 -type f -name "*.txt" -delete

# Remove all .py files
find . -maxdepth 1 -type f -name "*.py" -delete

# Remove input.c if it exists
if [ -e "input.c" ]; then
    rm "input.c"
    echo "Removed input.c"
fi

# Remove run.sh if it exists
if [ -e "run.sh" ]; then
    rm "run.sh"
    echo "Removed run.sh"
fi

# Remove command.sh if it exists
if [ -e "command.sh" ]; then
    rm "command.sh"
    echo "Removed command.sh"
fi

echo "Cleanup completed in the current directory."
