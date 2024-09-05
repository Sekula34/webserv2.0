import subprocess
from colors import Colors

relative_path = "../../cgi-bin/python3/"
og_file = relative_path + "link_dummy.py"
soft_link = relative_path + "soft.py"
hard_link = relative_path + "hard.py"


def create_python_test_files():
	create_link(original_file= og_file, link_name=soft_link)
	create_link(original_file=og_file, link_name=hard_link, linkType="hard")


def delete_python_test_files():
	delete_file(soft_link)
	delete_file(hard_link)

def create_link(original_file, link_name, linkType = "soft"):
	try:
		if(linkType == "soft"):
			print(Colors.color_text("Creating soft link", Colors.OKCYAN))
			subprocess.run(['ln', '-s', original_file, link_name], check= True)
			output_text = "Created soft link "
		else:
			print(Colors.color_text("Creating hard link", Colors.OKCYAN))
			subprocess.run(['ln', original_file, link_name], check= True)
			output_text = "Created hard link "
		print("{0} {1} that links to -> {2}".format(output_text ,link_name, original_file))
	except subprocess.CalledProcessError as e:
		print(f"Fail to create symbolid link: {e}")

def delete_file(filename):
	try:
		print(Colors.color_text("Deleting file " + filename, Colors.WARNING))
		subprocess.run(['rm', '-rf', filename], check=True)
		print(Colors.color_text("Delete complete", Colors.OKCYAN))
	except subprocess.CalledProcessError as e:
		print(f"Failed to delete file: {e}")

if __name__ == "__main__":
	create_python_test_files()
	#delete_python_test_files()
