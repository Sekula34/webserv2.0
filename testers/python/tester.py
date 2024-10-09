import requests
import unittest
from colors import Colors
import CustomRequst
import time
import utils
import chunkrequest

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
	def send_get(url, allow_redirects = True):
		start = time.time()
		response = requests.get(url, allow_redirects=allow_redirects)
		end = time.time()
		miliseconds = round((end - start) * 1000, 3)
		print("Response took: {0} milliseconds(ms)".format(miliseconds))
		return response
	
	@staticmethod
	def send_post(url, file_path):

		with open(file_path, 'rb') as pdf_file:
		# Use the files parameter to send the file
			files = {
				'file': (file_path, pdf_file, 'application/pdf')
			}
			start = time.time()
			response = requests.post(url, files=files)
			end = time.time()
		miliseconds = round((end - start) * 1000, 3)
		print("Response took: {0} milliseconds(ms)".format(miliseconds))
		return response
	
	@staticmethod
	def send_delete(url):
		start = time.time()
		response = requests.delete(url)
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
		respones = TestMyWebServer.send_get("http://localhost:9090/secondSite.html")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Second Site", respones.text)
		respones = TestMyWebServer.send_get("http://localhost:9090/subfolder/thirdSite.html")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Third Site", respones.text)
		Colors.test_passed()

	def test_autoindex(self):
		TestMyWebServer.print_test_title("Testing autoindex")
		response = TestMyWebServer.send_get("http://localhost:9090/autoindex/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Auto index of folder:", response.text)
		Colors.test_passed()

	def test_autoindexSubfolder(self):
		TestMyWebServer.print_test_title("Testing autoindex subfolder")
		response = TestMyWebServer.send_get("http://localhost:9090/autoindex/subfolder/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Auto index of folder:", response.text)
		Colors.test_passed()

	def test_autoindexThatHaveIndex(self):
		TestMyWebServer.print_test_title("Testing autoindex that have landing page")
		response = TestMyWebServer.send_get("http://localhost:9090/autoindexPage/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("No auto index here", response.text)
		Colors.test_passed()

	def test_autoindexBlocked(self):
		TestMyWebServer.print_test_title("Testing autoindex that is blocked")
		response = TestMyWebServer.send_get("http://localhost:9090/autoindexBlocked/")
		self.assertEqual(response.status_code, 404)
		self.assertIn("404", response.text)
		Colors.test_passed()

	def test_redirection(self):
		TestMyWebServer.print_test_title("Testing redirection")
		response = TestMyWebServer.send_get("http://localhost:9090/redirectMe/", allow_redirects=False)
		self.assertEqual(response.status_code, 302)
		response = TestMyWebServer.send_get("http://localhost:9090/redirectMe/", allow_redirects=True)
		self.assertEqual(response.status_code, 200)
		self.assertIn("You have been redirected!", response.text)
		response = TestMyWebServer.send_get("http://localhost:9090/redirectMeWrong/", allow_redirects=True)
		self.assertEqual(response.status_code, 404)
		Colors.test_passed()

	def test_post(self):
		TestMyWebServer.print_test_title("Testing post request")
		response = TestMyWebServer.send_post("http://localhost:9090/upload/?file.pdf", "pdfexample.pdf")
		self.assertEqual(response.status_code, 201)
		TestMyWebServer.print_test_title("Testing delete request")
		response = TestMyWebServer.send_delete("http://localhost:9090/upload/uploadFolder/file.pdf")
		self.assertEqual(response.status_code, 204)
		Colors.test_passed()

	def test_chunk_post(self):
		TestMyWebServer.print_test_title("Testing chunk post valid")
		response = chunkrequest.send_chunked_post(path = "/upload/?temp.png")
		self.assertEqual(response.status, 201)
		TestMyWebServer.send_delete("http://localhost:9090/upload/uploadFolder/temp.png")
		TestMyWebServer.print_test_title("Testing chunk limit invalid")
		response = chunkrequest.send_chunked_post(path = "/uploadLimit/")
		self.assertEqual(response.status, 413)
		Colors.test_passed()

	def test_body_size_limit(self):
		TestMyWebServer.print_test_title("Testing body size limit")
		url = "http://localhost:9090/uploadLimit/?100chars.txt"
		# Data to send: 100 characters
		data = "a" * 100  # You can modify this with whatever content you want
		# Send POST request
		response = requests.post(url, data=data)
		self.assertEqual(response.status_code, 201)
		Colors.test_passed()
		response = TestMyWebServer.send_delete("http://localhost:9090/uploadFolder/100chars.txt")
		data = "a" * 101
		response = requests.post(url, data=data)
		self.assertEqual(response.status_code, 413)
		Colors.test_passed()

	def test_url_decode(self):
		TestMyWebServer.print_test_title("Testing url decoding")
		response = TestMyWebServer.send_get("http://localhost:9090/fancy%20space%20.html")
		self.assertEqual(response.status_code, 200)
		self.assertIn("space in file name", response.text)
		Colors.test_passed()

	def test_phantom_port(self):
		TestMyWebServer.print_test_title("Testing phantom port")
		response = CustomRequst.phantom_port()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
		Colors.test_passed()
	
	def test_disguise_port(self):
		TestMyWebServer.print_test_title("Testing disguise port")
		response = CustomRequst.disguise_port()
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
	
	def test_contradict_content_length_transfer_encoding(self):
		TestMyWebServer.print_test_title("Testing contradict Content-Length and Transfer-Encoding")
		response = CustomRequst.transfer_encoding_and_content_length()
		self.assertTrue(response.startswith("HTTP/1.1 400 Bad Request"))
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
		cgi_scripts = {"hard.py" : 500, "io_test.py" : 200, "kill.py" : 504, "link_dummy.py" : 500, "loop.py" : 504, "soft.py" : 500}
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
