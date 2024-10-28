#!/usr/bin/env python3

import os

print("Content-Type: text/html\n")

# Get the directory path from PATH_INFO
directory_path = os.getenv("PATH_INFO", "")

# Check if PATH_INFO is not empty
if directory_path:
    # Build the full path to text.txt in the specified directory
    file_path = os.path.join(directory_path, "text.txt")
    
    try:
        # Open and read the contents of text.txt
        with open(file_path, "r") as file:
            contents = file.read()
            print(contents)
    except FileNotFoundError:
        print("Error: text.txt not found in the specified directory.")
        print(file_path)
    except Exception as e:
        print(f"Error: {e}")
else:
    print("Error: PATH_INFO is not set.")

