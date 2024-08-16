import socket
from colors import Colors

host ="localhost"
port = 9090

def badRequest():
	text = Colors.color_text("Sending Bad Request", Colors.FAIL)
	print(text)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, port))
	curl_request = "GET / HTTP/1.1\r\nHost: example.com\r\nUser-Agent: CustomClient\r\n\r\n"
	sock.sendall(curl_request.encode("utf-8"))
	server_response = sock.recv(4096)
	print(server_response.decode("utf-8"))
	sock.close()

def main():
	badRequest()

if __name__ == "__main__":
	main()
