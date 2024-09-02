#!/bin/bash

# Define the C++ source file and output comparison file
SOURCE_FILE="*.cpp"
OUTPUT_FILE="output.txt"
TEMP_OUTPUT="temp_output.txt"

# Initialize MAXLINE value
MAXLINE=1
MAX_LIMIT=5

# Flag to track if any mismatch occurred
mismatch_found=false

# Loop to increase MAXLINE and compile, execute, and compare outputs
while [ $MAXLINE -le $MAX_LIMIT ]; do
    # Compile the program with the current MAXLINE value
    g++ -o program -DMAXLINE=$MAXLINE $SOURCE_FILE
    
    # Check if compilation succeeded
    if [ $? -ne 0 ]; then
        echo "Compilation failed at MAXLINE=$MAXLINE"
        exit 1
    fi

    # Run the compiled program and capture its output
    ./program > $TEMP_OUTPUT
    
    # Compare the output with the predefined output file
    if cmp -s "$TEMP_OUTPUT" "$OUTPUT_FILE"; then
        echo "Output matches for MAXLINE=$MAXLINE	✅"
    else
        echo "Output does not match for MAXLINE=$MAXLINE	❌"
        mismatch_found=true
        break  # Exit loop if a mismatch is found
    fi
    
    # Increment MAXLINE
    MAXLINE=$((MAXLINE + 1))
done

# Check if any mismatch was found
if [ "$mismatch_found" = true ]; then
    echo "Mismatch found. Exiting script early."
else
    echo "No mismatches found. Reached MAXLINE limit of $MAX_LIMIT."
fi

# Cleanup temporary files
rm -f program $TEMP_OUTPUT

echo "Script finished. Final MAXLINE value was $MAXLINE."

