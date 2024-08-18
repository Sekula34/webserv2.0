import tester
import unittest
from colors import Colors



def tester_intro():
	text = Colors.color_text("Testers is about to run, make sure webserver is running", Colors.HEADER)
	print(text)
	inputText = Colors.color_text("Press Enter to Continue ", Colors.BOLD)
	input(inputText)

def spammer(testInstance):
	question = Colors.color_text("Do you want to run spammer?\n\t1 - Yes\n\t0 - NO\nAnswer: ", Colors.BOLD)
	user_answer = input(question)
	if(user_answer == "1"):
		spammerInput(testInstance)
	else:
		print("Spammer test is skipped")
		

def spammerInput(testInstance):
	question = Colors.color_text("How many request you want to send?[leave empty for default case of 500]: ", Colors.BOLD)
	answer = input(question)
	if(answer == ""):
		answer = 500
	testInstance.spammer(int(answer))


if __name__ == "__main__":
	#used to load test cases from specific module
	tester_intro()
	loader = unittest.TestLoader()
	suite = loader.loadTestsFromModule(tester)

	runner = unittest.TextTestRunner()
	runner.run(suite)

	testInstance = tester.TestMyWebServer()
	spammer(testInstance=testInstance)
	#testInstance.spammer()
