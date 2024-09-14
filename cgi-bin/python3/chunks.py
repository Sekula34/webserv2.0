#!/usr/bin/env python3

import sys
import time

# Print headers, including Transfer-Encoding: chunked
print("Transfer-Encoding: chunked")
print("Content-Type: text/html\n")

# The HTML content to be sent
html_content = """
<html>
<head><title>Hello World CGI</title></head>
<body><h2>this is used to be chunky but now it's not!</h2></body>
</html>
"""

# Function to write content in proper HTTP chunked encoding
def write_chunked(content, chunk_size=10):
    for i in range(0, len(content), chunk_size):
        chunk = content[i:i + chunk_size]
        chunk_len = hex(len(chunk))[2:]  # Convert chunk size to hexadecimal
        sys.stdout.write(f"{chunk_len}\r\n")  # Write chunk size in hex
        sys.stdout.write(f"{chunk}\r\n")      # Write the actual chunk
        sys.stdout.flush()                    # Flush to send immediately
        #time.sleep(0.5)  # Simulate delay for each chunk

    # Send the final 0 chunk to indicate the end
    sys.stdout.write("0\r\n\r\n")
    sys.stdout.flush()

# Send the content in chunks
write_chunked(html_content)


