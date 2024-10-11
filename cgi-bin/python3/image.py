#!/usr/bin/env python3

import os
import sys

def main():
	# Path to the image file
	image_path = "image.jpg"

	try:
		# Open the image file in binary mode
		with open(image_path, 'rb') as image_file:
			image_data = image_file.read()

		# Output the necessary CGI headers for an image response
		print("Content-Type: image/jpeg")
		print(f"Content-Length: {len(image_data)}")  # Optional, but helpful
		print()  # End of headers

		# Write the image data as the response body
		sys.stdout.buffer.write(image_data)

	except FileNotFoundError:
		# Handle file not found error gracefully
		print("Status: 404 Not Found")
		print("Content-Type: text/html")
		print()
		print("<h1>404 Not Found</h1><p>The requested image could not be found.</p>")

if __name__ == "__main__":
	main()

