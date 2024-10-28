#!/usr/bin/env python3

import os
import cgi
import cgitb
import html
from datetime import datetime  # Import datetime

cgitb.enable()  # Enable debugging messages in Browser

# Define the path to your log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
	if message == "=============================":
		current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
		message = f"{current_time} : {message}"
	with open(log_file, "a") as log:
		log.write(f"{message}\n")

log_message("=============================")
log_message("search_name.py script started")

# Function to dynamically build paths based on the server's document root
def build_path(relative_path):
	# Constructs a full path based on the server's root directory.
	# If I want a fallback document, I can use: root_dir = os.environ.get('DOCUMENT_ROOT', '../../www/basic-website/public')
	# relative_prefix = "../../" # Path traversal so the script can access www folder.
	root_dir = os.environ['DOCUMENT_ROOT']
	# return os.path.join(relative_prefix, root_dir, relative_path)
	return os.path.join(root_dir, relative_path)

# Print the Content-Type header for HTTP response
print("Content-Type: text/html\n")

# Get the form data
form = cgi.FieldStorage()
search_name = form.getvalue("name")

if search_name:
	safe_search_name = html.escape(search_name)
	# Construct the dynamic path to database.html
	database_file = build_path('pages/database.html')
	result_found = False

	# Check if the script can access the file
	if not os.access(database_file, os.R_OK | os.W_OK):
		log_message(f"Error: Script does not have read/write access to {database_file}")
	else:
		log_message(f"Script has read/write access to {database_file}")

	log_message(f"Searching for name: {safe_search_name} in {database_file}")

	try:
		with open(database_file, "r") as file:
			lines = file.readlines()
			for line in lines:
				if safe_search_name in line:
					print(f"<p>Found: {line.strip()}</p>")
					result_found = True
					log_message(f"Name '{safe_search_name}' found in the database.")
					break
		if not result_found:
			print("<p>No matching name found.</p>")
			log_message(f"Name '{safe_search_name}' not found in the database.")
	except Exception as e:
		error_message = f"Error searching for name: {str(e)}"
		print(f"<p>{error_message}</p>")
		log_message(error_message)
else:
	print("<p>Please enter a name to search.</p>")
	log_message("No name provided for search.")

# ============================================================================
# Old script that didnt build dinamically the path to database, nor have the logging function.
"""
import cgi
import cgitb
import html

cgitb.enable()

print("Content-Type: text/html\n")

# Get the form data
form = cgi.FieldStorage()
search_name = form.getvalue("name")

if search_name:
	safe_search_name = html.escape(search_name)
	database_file = "../../www/basic-website/public/pages/database.html"
	result_found = False

	try:
		with open(database_file, "r") as file:
			lines = file.readlines()
			for line in lines:
				if safe_search_name in line:
					print(f"<p>Found: {line.strip()}</p>")
					result_found = True
					break
		if not result_found:
			print("<p>No matching name found.</p>")
	except Exception as e:
		print(f"<p>Error searching for name: {str(e)}</p>")
else:
	print("<p>Please enter a name to search.</p>")
 """
