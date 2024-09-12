import http.client

# Define the server and endpoint
host = "localhost"
port = 9090
endpoint = "/"

# Create the connection with the specified port
conn = http.client.HTTPConnection(host, port)

# Start the request
conn.putrequest("POST", endpoint)
conn.putheader("Host", host)
conn.putheader("Transfer-Encoding", "chunked")
conn.putheader("Content-Type", "text/plain")
conn.endheaders()

# Function to send a chunk
def send_chunk(conn, chunk):
    # Send the size of the chunk in hexadecimal format followed by CRLF
    conn.send(f"{len(chunk):X}\r\n".encode())
    # Send the actual chunk data followed by CRLF
    conn.send(chunk.encode() + b"\r\n")

# Send each chunk
send_chunk(conn, "hello")
send_chunk(conn, "there")
send_chunk(conn, "wtf")

# Send the final chunk to indicate the end of the data
conn.send(b"0\r\n\r\n")

# Get the response
response = conn.getresponse()
print(response.status, response.reason)

# Read and print the response body
data = response.read()
print(data.decode())

# Close the connection
conn.close()

