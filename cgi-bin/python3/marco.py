#!/usr/bin/env python3
import os

# Get the content to print
content = f"Current Working Directory: {os.getcwd()}"

# Print headers
print("Status: 200 OK", end="\r\n")
print("Content-Type: text/html", end="\r\n")
print(f"Content-Length: {len(content)}", end="\r\n")
print("Language: English", end="\r\n\r\n")

# Print the actual content
print(content)
