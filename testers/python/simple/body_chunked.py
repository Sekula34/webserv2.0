import http.client

# Define the connection
conn = http.client.HTTPConnection("localhost", 9090)  # Replace 'example.com' with your server's host

# Headers for the POST request (with 'Transfer-Encoding: chunked')
headers = {
    'Content-type': 'text/plain',
    'Transfer-Encoding': 'chunked',
}

# Start the request without sending the body yet
conn.putrequest("POST", "/cgi-bin/hello.py")
for header, value in headers.items():
    conn.putheader(header, value)
conn.endheaders()

# Function to send data chunks in HTTP chunked encoding format
def send_chunk(connection, data):
    chunk_size = format(len(data), 'X')  # Size of the chunk in hex
    connection.send(chunk_size.encode('utf-8'))  # Send chunk size in hex
    connection.send(b"\r\n")  # Followed by CRLF
    connection.send(data.encode('utf-8'))  # Send the actual data
    connection.send(b"\r\n")  # End the chunk with CRLF

# Sending body in chunks ("hi " + "there")
send_chunk(conn, "hi ")
send_chunk(conn, "there!")
send_chunk(conn, " How are you?")

# Signal the end of the chunked transfer
conn.send(b"0\r\n\r\n")  # 0 followed by CRLF indicates the end of the chunks

# Get the response
response = conn.getresponse()

# Print the response status and body
print(response.status, response.reason)
print(response.read().decode())

# Close the connection
conn.close()

