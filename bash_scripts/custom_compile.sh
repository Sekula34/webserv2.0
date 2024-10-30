#!/bin/bash

# Check if the user has provided a filename
if [ -z "$1" ]; then
  echo "Usage: $0 <filename.cpp>"
  exit 1
fi

# Extract the filename without the extension
filename="${1%.*}"

# Compile the C++ file as a shared object
g++ -shared -fPIC "$1" -ldl -o "$filename.so"

# Check if the compilation was successful
if [ $? -eq 0 ]; then
  echo "Successfully compiled $1 to $filename.so"
else
  echo "Compilation failed."
fi
