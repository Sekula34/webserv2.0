class Colors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKCYAN = '\033[96m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'
	BOLD = '\033[1m'
	UNDERLINE = '\033[4m'

	@staticmethod
	def color_text(text, color):
		if isinstance(color, str) and hasattr(Colors, color):
			color_code = getattr(Colors, color)
		else :
			color_code = color
		return(f"{color_code}{text}{Colors.ENDC}")
	
	@staticmethod
	def test_case_title(text):
		return(f"{Colors.OKBLUE}{text}{Colors.ENDC}")
	
	@staticmethod
	def test_passed(message = ""):
		print(f"{Colors.OKGREEN}Test passed ✅ {Colors.ENDC}{message}")
	
	@staticmethod
	def test_failed(message = ""):
		print(f"{Colors.FAIL}Test failed ❌ {Colors.ENDC}{message}")

	@staticmethod 
	def print_warning(text):
		colored_text = Colors.color_text(text, Colors.WARNING)
		print(text)

		


	
