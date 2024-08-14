import tester
import unittest
from colors import Colors


if __name__ == "__main__":
	#used to load test cases from specific module
	Colors.color_print("Testers is about to run, make sure webserver is running", Colors.HEADER)
	loader = unittest.TestLoader()
	suite = loader.loadTestsFromModule(tester)

	runner = unittest.TextTestRunner()
	runner.run(suite)

	testInstance = tester.TestMyWebServer()
	testInstance.spammer()
