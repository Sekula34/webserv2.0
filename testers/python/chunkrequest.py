#!/usr/bin/env python3

import http.client
import os

def send_chunked_post(url = "localhost", port = 9090, path = "/", file_path = "image.png", chunk_size = 100):
	conn = http.client.HTTPConnection(url, port)
	
	# Open the file for reading in binary mode
	with open(file_path, 'rb') as f:
		# Initiate the request with chunked transfer
		conn.putrequest("POST", path)  # Use the path variable
		conn.putheader("Transfer-Encoding", "chunked")
		conn.putheader("Content-Type", "application/octet-stream")
		conn.endheaders()

		# Send file in chunks
		while True:
			chunk = f.read(chunk_size)
			if not chunk:
				break
			conn.send(f"{len(chunk):X}\r\n".encode())  # Send the chunk size in hex
			conn.send(chunk)
			conn.send(b"\r\n")  # End of chunk

		# Final chunk (zero length) to indicate end of data
		conn.send(b"0\r\n\r\n")

	# Get the response
	response = conn.getresponse()
	conn.close()
	return response

if __name__ == "__main__":
	# Usage example:
	# Modify these values for your testing
	url = "localhost" # Replace with your server address
	#port = 9494 # Replace with your server port (Nginx or your own webserver)
	port = 9090
	path = "/" # Replace with the website path
	file_path = "image.png"  # "path/to/your/file.txt" Path to the file to be uploaded
	chunk_size = 100   # Chunk size in bytes 1024

	response = send_chunked_post()
	print(f"Response status: {response.status}")
	print(f"Response reason: {response.reason}")
	print("Current Working Directory:", os.getcwd())
