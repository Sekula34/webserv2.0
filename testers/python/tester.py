import requests
import unittest

class TestMyWebServer(unittest.TestCase):
	def setUp(self):
		pass
	def tearDown(self):
		pass

	def test_home_page(self):
		response = requests.get("http://localhost:9090/")
		self.assertEqual(response.status_code, 200)
		self.assertIn("Welcome to Server 9090", response.text)
	
	def test_404_page(self):
		response = requests.get("http://localhost:9090/idondtexist/")
		self.assertEqual(response.status_code, 404)
	
	def test_405_page(self):
		response = requests.put("http://localhost:9090/")
		self.assertEqual(response.status_code, 405)
	
	def test_other_file(self):
		respones = requests.get("http://localhost:8080/hej/Socket.cpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)
		respones = requests.get("http://localhost:8080/hej/Socket.hpp")
		self.assertEqual(respones.status_code, 200)
		self.assertIn("Socket", respones.text)

	def spammer(self, numberOfRequest = 500):
		accepted_count = 0
		print("running spammer")
		for i in range (0, numberOfRequest):
			try:
				response = requests.get("http://localhost:8080/hej/Socket.hpp", timeout=0.1)
				if response.status_code == 200:
					accepted_count += 1
			except requests.exceptions.RequestException as e:
				print(f"request {i} failed: {e}")

			#self.assertEqual(response.status_code, 200)
		print("Spammer done. Accepted Request {0}/{1}".format(accepted_count, numberOfRequest))
