#!/bin/bash

# Check if at least one argument is provided
if [ $# -eq 0 ]; then
    echo "No arguments provided. Usage: ./leak.sh <argument> example ./leak.sh configuration_files/default.conf"
    exit 1
fi

# Replace 'your_program' with the name or path of your program
valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes ./webserv "$@"
