import socket
from colors import Colors
import time 

HOST ="localhost"
PORT = 9090

class CustomRequest():
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
		start = time.time()
		sock.connect((host, port))
		sock.sendall(self.__curl_request.encode("utf-8"))
		server_response = sock.recv(4096)
		decoded_response = server_response.decode("utf-8")
		#print(decoded_response)
		sock.close()
		end = time.time()
		miliseconds = round((end - start) * 1000, 3)
		print("Response took: {0} milliseconds(ms)".format(miliseconds))
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


def short_invalid_request():
	req = CustomRequest("Short Invalid Request", "GET\r\n", 400)
	return(req.send())

def phantom_port():
	curl_request = "GET / HTTP/1.1\r\nHost: example.com\r\nUser-Agent: CustomClient\r\n\r\n"
	req = CustomRequest("Confusing Host", curl_request, 400)
	return(req.send())

def disguise_port():
	curl_request = "GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: CustomClient\r\n\r\n"
	req = CustomRequest("Confusing Host", curl_request, 400)
	return(req.send())

def no_host():
	curl_request = "GET / HTTP/1.1\r\nUser-Agent: CustomClient\r\n\r\n"
	req = CustomRequest("No Host", curl_request, 400)
	return(req.send())

def authorization_not_supported():
		curl_request = "GET / HTTP/1.1\r\n\
Host:localhost:9090\r\n\
Authorization: SomeAuthorization\r\n\
User-Agent: CustomClient\r\n\r\n"
		req = CustomRequest("Authorization", curl_request, 403)
		return(req.send())

def main():
	response = authorization_not_supported()
	print("Authorization reposne is {0}".format(response))
	response = no_host()
	print("NO hosT reposne is {0}".format(response))
	message = phantom_port()
	print("Message is [{0}]".format(message))
	short_invalid_request()

if __name__ == "__main__":
	#main()
	message = disguise_port()
	print(message)
