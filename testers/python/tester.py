import requests
import unittest
from colors import Colors
import badRequest

class TestMyWebServer(unittest.TestCase):
	def setUp(self):
		pass
	def tearDown(self):
		pass

	@staticmethod
	def print_test_title(text):
		title = Colors.test_case_title("\nTesting:" + text)
		print(title)

	def test_home_page(self):
		TestMyWebServer.print_test_title("Home page")
		response = requests.get("http://localhost:9090/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Welcome to Server 9090", response.text)
		Colors.test_passed()
	
	def test_404_page(self):
		TestMyWebServer.print_test_title("Error 404")
		response = requests.get("http://localhost:9090/idondtexist/")
		self.assertEqual(response.status_code, 404)
		Colors.test_passed()
	
	def test_405_page(self):
		TestMyWebServer.print_test_title("Error 405")
		response = requests.put("http://localhost:9090/")
		self.assertEqual(response.status_code, 405)
		Colors.test_passed()
	
	def test_other_file(self):
		TestMyWebServer.print_test_title("Files that are not part of location")
		respones = requests.get("http://localhost:8080/hej/Socket.cpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)
		respones = requests.get("http://localhost:8080/hej/Socket.hpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)
		Colors.test_passed()

	def test_bad_request(self):
		TestMyWebServer.print_test_title("Testing phantom port")
		response = badRequest.phantom_port()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
		Colors.test_passed()
		


	def spammer(self, numberOfRequest = 500):
		accepted_count = 0
		TestMyWebServer.print_test_title("Spammer on " + str(numberOfRequest))
		for i in range (0, numberOfRequest):
			try:
				response = requests.get("http://localhost:8080/hej/Socket.hpp", timeout=0.1)
				if response.status_code == 200:
					accepted_count += 1
			except requests.exceptions.RequestException as e:
				print(f"request {i} failed: {e}")
		self.__spammerResult(numberOfRequest, accepted_count)

	def __spammerResult(self, numberOfRequst, accpted):
		result = "Spammer done {0}/{1} test accepted".format(accpted, numberOfRequst)
		if(accpted == numberOfRequst):
			result = Colors.color_text(result, Colors.OKGREEN)
		else:
			result = Colors.color_text(result, Colors.FAIL)
		print(result)
