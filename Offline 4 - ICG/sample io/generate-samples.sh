#!/bin/bash

# Set the path to the input folder
input_folder="./input"

# Set the path to the samples folder
samples_folder="./samples"

# Create the samples folder if it doesn't exist
mkdir -p "$samples_folder"

# Process each C file in the input folder
for c_file in "$input_folder"/*.c; do
    # Check if there are any C files
    if [ -e "$c_file" ]; then
        # Extract the file name (without extension)
        base_name=$(basename -s .c "$c_file")

        # Compile the C file and generate code.asm
        ./a.out $c_file

        # Copy the C code to a text file in the samples folder
        txt_file_path="$samples_folder/$base_name.txt"
        cp "$c_file" "$txt_file_path"

        # Copy the generated ASM code to a file in the samples folder
        asm_file_path="$samples_folder/$base_name.asm"
        cp code.asm "$asm_file_path"
    fi
done

echo "Processing complete. Check the 'samples' folder for the results."
