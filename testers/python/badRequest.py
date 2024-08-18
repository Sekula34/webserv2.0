import socket
from colors import Colors
import time 

HOST ="localhost"
PORT = 9090

class RequestData():
	def __init__(self, title, curl_request, expected_http_status):
		self.__title = title
		self.__curl_request = curl_request
		self.__expecetd_http_status = expected_http_status

	def print(self):
		mesage = "Data info:\n\ttitle:[{0}]\n\
				\r\trequest:[{1}]".format(self.__title, self.__curl_request)
		text = Colors.color_text(mesage, Colors.OKCYAN)
		print(text)

	def send(self, port = 9090, host = "localhost"):
		text = Colors.color_text("Sending custom request", Colors.WARNING)
		print(text)
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect((host, port))
		sock.sendall(self.__curl_request.encode("utf-8"))
		server_response = sock.recv(4096)
		decoded_response = server_response.decode("utf-8")
		print(decoded_response)
		sock.close()
		return decoded_response
		


def badRequest():
	host = "localhost"
	port = 9090
	text = Colors.color_text("Sending Bad Request", Colors.FAIL)
	print(text)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, port))
	curl_request = "GET / HTTP/1.1\r\nHost: example.com\r\nUser-Agent: CustomClient\r\n\r\n"
	sock.sendall(curl_request.encode("utf-8"))
	server_response = sock.recv(4096)
	print(server_response.decode("utf-8"))
	sock.close()

def no_host_request():
	text = Colors.color_text("Sending requsest with no host", Colors.FAIL)
	print(text)


def long_timeout():
	req = RequestData("Long Timeout", "get", 400)
	start = time.time()
	req.send()
	end = time.time()
	print("Response took: {0} seconds".format(end - start))

def phantom_port():
	curl_request = "GET / HTTP/1.1\r\nHost: example.com\r\nUser-Agent: CustomClient\r\n\r\n"
	req = RequestData("Confusing Host", curl_request, 400)
	return(req.send())

def main():
	message = phantom_port()
	print("Message is [{0}]".format(message))
	#long_timeout()

if __name__ == "__main__":
	main()
