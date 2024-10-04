import socket

# Define the server address and port
# server_address = ('localhost', 9090)
server_address = ('localhost', 80)

# Create a TCP socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to the server
    sock.connect(server_address)
    
    # Send the invalid request containing only "test"
    message = b"GET / HTTP/1.1"
    # message = b"hi"
    sock.sendall(message)

    # Try to receive the response (if any)
    response = sock.recv(4096)

# Print the raw response from the server
print("Response:", response.decode())

