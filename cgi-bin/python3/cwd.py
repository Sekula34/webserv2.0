#!/usr/bin/env python3

import os

print("Content-Type: text/html\n")

# Get the current working directory
cwd = os.getcwd()

# Print a simple HTML page with the working directory in a paragraph
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Current Working Directory</title>
</head>
<body>
    <h1>Current Working Directory</h1>
    <p>{cwd}</p>
</body>
</html>
""")

