import tester
import unittest

if __name__ == "__main__":
	#used to load test cases from specific module
	loader = unittest.TestLoader()
	suite = loader.loadTestsFromModule(tester)

	runner = unittest.TextTestRunner()
	runner.run(suite)

	testInstance = tester.TestMyWebServer()
	testInstance.test_spammer()
