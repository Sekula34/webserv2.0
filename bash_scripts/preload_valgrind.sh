#!/bin/bash

# Check if the user has provided a shared object file
if [ -z "$1" ]; then
  echo "Usage: $0 <shared_object.so>"
  exit 1
fi

# Run the web server with the provided shared object preloaded
LD_PRELOAD="./$1" valgrind --leak-check=full --show-leak-kinds=all ./webserv

# Check if the command was successful
if [ $? -eq 0 ]; then
  echo "Web server started with $1 preloaded."
else
  echo "Failed to start the web server with $1."
fi
