import tester
import unittest
from colors import Colors



def tester_intro():
	text = Colors.color_text("Testers is about to run, make sure webserver is running with correct Config file [configuration_files/valid/pythonTester.conf]", Colors.HEADER)
	print(text)
	inputText = Colors.color_text("Press Enter to Continue ", Colors.BOLD)
	input(inputText)

def spammer(testInstance):
	question = Colors.color_text("Do you want to run spammer?\n\t1 - Yes\n\t0 - NO\nAnswer: ", Colors.BOLD)
	user_answer = input(question)
	if(user_answer == "1"):
		output_text = Colors.color_text("Spamming on cgi", Colors.OKBLUE)
		print(output_text)
		spammerInput(testInstance, "http://localhost:9090/cgi-bin/io_test.py")
		output_text = Colors.color_text("\nSpamming on non cgi", Colors.OKBLUE)
		print(output_text)
		spammerInput(testInstance, "http://localhost:9090/")
	else:
		print("Spammer test is skipped")
		

def spammerInput(testInstance, url):
	question = Colors.color_text("How many request you want to send?[leave empty for default case of 50]: ", Colors.BOLD)
	answer = input(question)
	if(answer == ""):
		answer = 50
	testInstance.spammer(url, int(answer))


def big_main():
	tester_intro()
	loader = unittest.TestLoader()
	suite = loader.loadTestsFromModule(tester)

	runner = unittest.TextTestRunner()
	runner.run(suite)

	testInstance = tester.TestMyWebServer()
	spammer(testInstance=testInstance)

if __name__ == "__main__":
	big_main()
