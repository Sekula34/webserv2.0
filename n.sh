#!/usr/bin/bash

#!/bin/bash

# File containing requests
FILE="short_invalid_requests.txt"
SERVER="localhost"
PORT="9090"

# Variable to accumulate request content
request=""

# Read each line of the file
while IFS= read -r line || [ -n "$line" ]; do
    # If we encounter an empty line, it means the request block is finished
    if [[ -z "$line" ]]; then
        # Send the accumulated request to the server using netcat
        echo "$request" | nc "$SERVER" "$PORT"
        echo "Request sent: "
        echo "$request"
        echo "-------------------"
        # Clear the request for the next block
        request=""
    else
        # Accumulate the lines of the request
        request="$request$line"$'\r\n'
    fi
done < "$FILE"

# Send the last request if the file doesn't end with a blank line
if [[ -n "$request" ]]; then
    echo "$request" | nc "$SERVER" "$PORT"
    echo "Request sent: "
    echo "$request"
fi


