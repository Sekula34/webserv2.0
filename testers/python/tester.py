import requests
import unittest
from colors import Colors
import CustomRequst
import time
import utils

class TestMyWebServer(unittest.TestCase):
	def setUp(self):
		pass
	def tearDown(self):
		pass

	@staticmethod
	def print_test_title(text):
		title = Colors.test_case_title("\nTesting:" + text)
		print(title)

	@staticmethod
	def send_get(url):
		start = time.time()
		response = requests.get(url)
		end = time.time()
		miliseconds = round((end - start) * 1000, 3)
		print("Response took: {0} milliseconds(ms)".format(miliseconds))
		return response
	
	@staticmethod
	def cgi_prompt():
		question = Colors.color_text("Do you want to run cgi tester. Some test cases take 15 seconds because of timeout. \n\t 1 - Yes \n\t 2 - No\nAnswer: ", Colors.BOLD)
		answer = input(question)
		return answer

	def test_home_page(self):
		TestMyWebServer.print_test_title("Home page")
		response = TestMyWebServer.send_get("http://localhost:9090/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Welcome to Server 9090", response.text)
		Colors.test_passed()
	
	def test_404_page(self):
		TestMyWebServer.print_test_title("Error 404")
		response = TestMyWebServer.send_get("http://localhost:9090/idondtexist/")
		self.assertEqual(response.status_code, 404)
		Colors.test_passed()
	
	def test_405_page(self):
		TestMyWebServer.print_test_title("Error 405")
		response = requests.put("http://localhost:9090/")
		self.assertEqual(response.status_code, 405)
		Colors.test_passed()

	def test_server_no_location(self):
		TestMyWebServer.print_test_title("Server no location")
		response = TestMyWebServer.send_get("http://localhost:8989/")
		self.assertEqual(response.status_code, 200)
		Colors.test_passed()

	def test_server_no_location(self):
		TestMyWebServer.print_test_title("Server no location test 2")
		response = TestMyWebServer.send_get("http://localhost:8989/amanemoj/")
		self.assertEqual(response.status_code, 404)
		Colors.test_passed()
	
	def test_other_file(self):
		TestMyWebServer.print_test_title("Files that are not part of location")
		respones = TestMyWebServer.send_get("http://localhost:8080/hej/Socket.cpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)
		respones = TestMyWebServer.send_get("http://localhost:8080/hej/Socket.hpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)
		Colors.test_passed()

	def test_autoindex(self):
		TestMyWebServer.print_test_title("Testing autoindex")
		response = TestMyWebServer.send_get("http://localhost:8080/autoindex/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Auto index of folder:", response.text)
		Colors.test_passed()

	def test_autoindexSubfolder(self):
		TestMyWebServer.print_test_title("Testing autoindex subfolder")
		response = TestMyWebServer.send_get("http://localhost:8080/autoindex/subfolder/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Auto index of folder:", response.text)
		Colors.test_passed()

	def test_autoindexThatHaveIndex(self):
		TestMyWebServer.print_test_title("Testing autoindex that have landing page")
		response = TestMyWebServer.send_get("http://localhost:8080/hej/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Socket.hpp", response.text)
		Colors.test_passed()

	def test_autoindexBlocked(self):
		TestMyWebServer.print_test_title("Testing autoindex that is blocked")
		response = TestMyWebServer.send_get("http://localhost:8080/autoindexBlocked/")
		self.assertEqual(response.status_code, 404)
		self.assertIn("404", response.text)
		Colors.test_passed()

	def test_url_decode(self):
		TestMyWebServer.print_test_title("Testing url decoding")
		response = TestMyWebServer.send_get("http://localhost:8080/autoindex/subfolder/frontend/fancy%20space%20.html")
		self.assertEqual(response.status_code, 200)
		self.assertIn("space in file name", response.text)
		Colors.test_passed()

	def test_double_valid_listen(self):
		TestMyWebServer.print_test_title("Testing double listen")
		response = TestMyWebServer.send_get("http://localhost:2020/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("I listen to both", response.text)
		response = TestMyWebServer.send_get("http://localhost:2222/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("I listen to both", response.text)
		Colors.test_passed()

	def test_phantom_port(self):
		TestMyWebServer.print_test_title("Testing phantom port")
		response = CustomRequst.phantom_port()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
		Colors.test_passed()
	
	def test_no_host(self):
		TestMyWebServer.print_test_title("Testing no host")
		response = CustomRequst.no_host()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
		Colors.test_passed()

	def test_authorization(self):
		TestMyWebServer.print_test_title("Testing authorization in header")
		response = CustomRequst.authorization_not_supported()
		self.assertTrue(response.startswith("HTTP/1.1 403 Forbidden"))
		Colors.test_passed()
	
	def test_short_invalid(self):
		TestMyWebServer.print_test_title("Testing short invalid request")
		response = CustomRequst.short_invalid_request()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
		Colors.test_passed()

	def test_all_cgi_scripts(self):
		TestMyWebServer.print_test_title("Testing cgi")
		if(TestMyWebServer.cgi_prompt() == "2"):
			Colors.print_warning("Cgi testing is skipped")
			return
		utils.create_python_test_files()
		cgi_scripts = {"hard.py" : 500, "io_test.py" : 200, "kill.py" : 500, "link_dummy.py" : 500, "loop.py" : 500, "soft.py" : 500}
		for script_name, status_code in cgi_scripts.items():
			self.__cgiTest("http://localhost:9090/cgi-bin/" + script_name, status_code, "")
		utils.delete_python_test_files()

	def spammer(self, url, numberOfRequest = 50):
		accepted_count = 0
		total_accept_time = 0
		TestMyWebServer.print_test_title("Spammer on " + str(numberOfRequest) + ": " + url)
		for i in range (0, numberOfRequest):
			try:
				start = time.time()
				response = requests.get(url, timeout=0.1)
				if response.status_code == 200:
					end = time.time()
					accepted_count += 1
					total_accept_time += end - start
			except requests.exceptions.RequestException as e:
				print(f"request {i} failed: {e}")
		self.__spammerResult(numberOfRequest, accepted_count, total_accept_time)

	def __spammerResult(self, numberOfRequst, accpted, total_accept_time):
		result = "Spammer done {0}/{1} test accepted".format(accpted, numberOfRequst)
		if(accpted != 0):
			avr_response_time_ms = round((total_accept_time/ accpted) * 1000, 3)
			result += ". Average time to get response in miliscedons is {0}".format(avr_response_time_ms)
		if(accpted == numberOfRequst):
			result = Colors.color_text(result, Colors.OKGREEN)
		else:
			result = Colors.color_text(result, Colors.FAIL)
		print(result)

	def __cgiTest(self, url, expected_status, expected_text):
		TestMyWebServer.print_test_title("Testing cgi: " + url)
		response = TestMyWebServer.send_get(url)
		self.assertEqual(response.status_code, expected_status)
		self.assertIn(expected_text, response.text)
