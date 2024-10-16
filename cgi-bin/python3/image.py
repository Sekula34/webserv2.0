#!/usr/bin/env python3

import os
import sys

# Path to the image file
image_filename = "image.png"
image_dir = os.environ.get('PATH_INFO')
image_path = os.path.join(image_dir, image_filename)

try:
    # Open the image file in binary mode
    with open(image_path, 'rb') as image_file:
        image_data = image_file.read()
    
    # Output the necessary CGI headers for an image response
    print("Content-Type: image/png")
    print(f"Content-Length: {len(image_data)}")  # Optional, but helpful
    print()  # End of headers
    # Flush the headers before sending the image data
    sys.stdout.flush()
	# Write the image data as the response body
    sys.stdout.buffer.write(image_data)

except FileNotFoundError:
    # Handle file not found error gracefully
    print("Status: 404 Not Found")
    print("Content-Type: text/html")
    print()
    print("<h1>404 Not Found</h1><p>The requested image could not be found.</p>")

