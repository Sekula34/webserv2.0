#!/bin/bash

# Check if the user has provided a C++ source file
if [ -z "$1" ]; then
  echo "Usage: $0 <filename.cpp>"
  exit 1
fi
SCRIPT_DIR="$(dirname "$0")"

# Get the filename without the extension
filename="${1%.*}"

# Call the compile_shared.sh script with the provided C++ file
./$SCRIPT_DIR/custom_compile.sh "$1"

# Check if compilation was successful
if [ $? -ne 0 ]; then
  echo "Compilation failed. Exiting."
  exit 1
fi

# Call the start_webserv.sh script with the generated shared object file
# Determine which preload script to use
if [ -z "$2" ]; then
  # No extra argument, use preload.sh
  "$SCRIPT_DIR/preload.sh" "$filename.so"
else
  # Extra argument provided, use preload_valgrind.sh
  "$SCRIPT_DIR/preload_valgrind.sh" "$filename.so"
fi
