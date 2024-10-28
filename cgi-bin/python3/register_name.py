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
log_message("register_name.py script started")

# Function to dynamically build paths based on the server's document root
def build_path(relative_path):
	# Constructs a full path based on the server's root directory.
	# If I want a fallback document, I can use: root_dir = os.environ.get('DOCUMENT_ROOT', '../../www/basic-website/public')
	# relative_prefix = "../../" # Path traversal so the script can access www folder.
	root_dir = os.environ['DOCUMENT_ROOT']
	return os.path.join(root_dir, relative_path)
	# return os.path.join(relative_prefix, root_dir, relative_path)

# Set the content-type header for HTTP response
print("Content-Type: text/html\n")
print("")

# Parse the form data for both GET and POST
form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")

# Safely escape the input to avoid HTML injection
if name and email:
	safe_name = html.escape(name)
	safe_email = html.escape(email)
	# Construct the dynamic path to database.html
	database_file = build_path('pages/database.html')

	# Check if the script can access the file
	if not os.access(database_file, os.R_OK | os.W_OK):
		log_message(f"Error: Script does not have read/write access to {database_file}")
	else:
		log_message(f"Script has read/write access to {database_file}")

	log_message(f"Registering: {safe_name} - {safe_email} in {database_file}")

	try:
		# Append the new name to the HTML file inside the <ul> element
		with open(database_file, "r+") as file:
			lines = file.readlines()
			ul_found = False
			
			# Find the line where the <ul id="namesList"> tag is located
			for i, line in enumerate(lines):
				if '<ul id="namesList">' in line:
					ul_found = True
					# Insert the new name just after the <ul> opening tag
					# print(f"<p>Inserting at line {i + 1} in database.html</p>")
					lines.insert(i + 1, f"        <li>Name: {safe_name}, Email: {safe_email}</li>\n")
					break
			
			if not ul_found:
				print("") # print("<p>Error: <ul id=\"namesList\"> not found in database.html</p>")
			else:
				# Move the file pointer to the beginning and write updated content
				file.seek(0)
				file.writelines(lines)
				print("<p>Name and email registered successfully!</p>")

	except Exception as e:
		# Output error message
		error_message = f"Error registering name and email: {str(e)}"
		print(f"<p>{error_message}</p>")
		log_message(error_message)
else:
	print("<p>Invalid data submitted.</p>")
	log_message("Invalid data submitted.")

# ==========================================================================
# Old script that didnt build dinamically the path to database, nor have the logging function.
""" 
import cgi
import cgitb
import html
import os

cgitb.enable()  # Enable debugging messages in Browser

# Set the content-type header for HTTP response
print("Content-Type: text/html\n")

# Parse the form data for both GET and POST
form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")

# Safely escape the input to avoid HTML injection
if name and email:
	safe_name = html.escape(name)
	safe_email = html.escape(email)

	# File path to the database HTML file
	database_file = "../../www/basic-website/public/pages/database.html"

	# Check if the script can access the file
	# if not os.access(database_file, os.R_OK | os.W_OK):
	# 	print(f"<p>Error: Script does not have read/write access to {database_file}</p>")
	# else:
	# 	print(f"<p>Script has read/write access to {database_file}</p>")

	try:
		# Append the new name to the HTML file inside the <ul> element
		with open(database_file, "r+") as file:
			lines = file.readlines()
			ul_found = False
			
			# Find the line where the <ul id="namesList"> tag is located
			for i, line in enumerate(lines):
				if '<ul id="namesList">' in line:
					ul_found = True
					# Insert the new name just after the <ul> opening tag
					# print(f"<p>Inserting at line {i + 1} in database.html</p>")
					lines.insert(i + 1, f"        <li>Name: {safe_name}, Email: {safe_email}</li>\n")
					break
			
			if not ul_found:
				print("") # print("<p>Error: <ul id=\"namesList\"> not found in database.html</p>")
			else:
				# Move the file pointer to the beginning and write updated content
				file.seek(0)
				file.writelines(lines)
				print("<p>Name and email registered successfully!</p>")

	except Exception as e:
		# Output error message
		print(f"<p>Error registering name and email: {str(e)}</p>")
else:
	print("<p>Invalid data submitted.</p>")
 """
# ==========================================================================

""" #!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()

# Define the path to your log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
    with open(log_file, "a") as log:
        log.write(f"{message}\n")

log_message("Script started")

# Print the Content-Type header for HTTP response
print("Content-Type: text/html\n")

# Log environment variables
content_length = os.environ.get('CONTENT_LENGTH', '0')
log_message(f"CONTENT_TYPE: {os.environ.get('CONTENT_TYPE')}")
log_message(f"CONTENT_LENGTH: {content_length}")
log_message(f"REQUEST_METHOD: {os.environ.get('REQUEST_METHOD')}")

# Initialize the FieldStorage object
form = cgi.FieldStorage()

# Inspect the keys and values parsed by FieldStorage
log_message(f"FieldStorage keys: {list(form.keys())}")
name = form.getvalue("name")
email = form.getvalue("email")
log_message(f"Extracted name: {name}, email: {email}")

# Output a simple message to the browser
print("<p>Check the server logs for details.</p>") """

# ==========================================================================

""" #!/usr/bin/env python3

import os
import sys
import cgitb
import cgi

cgitb.enable()  # Enable CGI debugging

# Define the path to your log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
    with open(log_file, "a") as log:
        log.write(f"{message}\n")

log_message("Script started")

# Log environment variables
log_message(f"CONTENT_TYPE: {os.environ.get('CONTENT_TYPE')}")
log_message(f"CONTENT_LENGTH: {os.environ.get('CONTENT_LENGTH')}")
log_message(f"REQUEST_METHOD: {os.environ.get('REQUEST_METHOD')}")

# Read raw input data
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
raw_data = sys.stdin.read(content_length)
log_message(f"Raw data received: {raw_data}")

# Try parsing the form data
form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")
log_message(f"Extracted name: {name}, email: {email}")

# Output a simple response
print("Content-Type: text/html\n")
print("<p>Check the server logs for details.</p>") """

# ==========================================================================

""" #!/usr/bin/env python3

import os
import sys
import cgi
import cgitb
import html

cgitb.enable()

# Define the path to your log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
    with open(log_file, "a") as log:
        log.write(f"{message}\n")

log_message("Script started")

# Print the Content-Type header for HTTP response
print("Content-Type: text/html\n")

# Check the request method
request_method = os.environ.get("REQUEST_METHOD", "GET")
log_message(f"Request method: {request_method}")

# Parse form data
form = cgi.FieldStorage()

if request_method == "POST":
    # Read the raw input data if needed
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    post_data = sys.stdin.read(content_length)
    log_message(f"Raw POST data: {post_data}")

    # Attempt to retrieve 'name' and 'email' from form data
    name = form.getvalue("name")
    email = form.getvalue("email")

    # Log the extracted form data
    log_message(f"Extracted name: {name}, email: {email}")
else:
    log_message("Not a POST request")

# Output a minimal message to the browser
print("<p>Check the server logs for details.</p>") """

# ==========================================================================

""" #!/usr/bin/env python3

import os
import cgi
import cgitb

cgitb.enable()  # Enable debugging

# Define the path to your log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
    with open(log_file, "a") as log:
        log.write(f"{message}\n")

# Log that the script started
log_message("Script started")

# Print the Content-Type header for HTTP response
print("Content-Type: text/html\n")

# Parse the form data for both GET and POST
form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")

# Log the received form data
log_message(f"Received name: {name}, email: {email}")

# File path to the database HTML file
database_file = "../../www/basic-website/public/pages/database.html"

# Check if the script can access the file and try to open it
try:
    if os.access(database_file, os.R_OK | os.W_OK):
        log_message(f"Script has read/write access to {database_file}")
    else:
        log_message(f"Error: Script does not have read/write access to {database_file}")

    # Attempt to open the file in read mode for a test
    with open(database_file, "r") as file:
        log_message("File opened successfully for reading.")

except Exception as e:
    log_message(f"Error occurred: {str(e)}")

# Output a minimal success message to the browser
print("<p>Check the server logs for details.</p>")
print(f"<p>Current Working Directory: {os.getcwd()}</p>") """

# ==========================================================================

""" #!/usr/bin/env python3

import cgi
import cgitb
import html

cgitb.enable()  # Enable debugging

# Set the content-type header for HTTP response
print("Content-Type: text/html\n")

# Parse the form data for both GET and POST
form = cgi.FieldStorage()
name = form.getvalue("name")
email = form.getvalue("email")

# Safely escape the input to avoid HTML injection
if name and email:
	safe_name = html.escape(name)
	safe_email = html.escape(email)

	# File path to the database HTML file
	# database_file = "path/to/your/database.html"  # Adjust the path to where your database.html is located
	database_file = "../../www/basic-website/public/pages/database.html"  # Adjust the path to where your database.html is located

	# Check if the script can access the file
	if not os.access(database_file, os.R_OK | os.W_OK):
		print(f"<p>Error: Script does not have read/write access to {database_file}</p>")
	else:
		print(f"<p>Script has read/write access to {database_file}</p>")

	try:
		# Append the new name to the HTML file inside the <ul> element
		with open(database_file, "r+") as file:
			lines = file.readlines()
			
			# Find the line where the <ul id="namesList"> tag is located
			for i, line in enumerate(lines):
				if '<ul id="namesList">' in line:
					# Insert the new name just after the <ul> opening tag
					lines.insert(i + 1, f"        <li>Name: {safe_name}, Email: {safe_email}</li>\n")
					break
			
			# Move the file pointer to the beginning and write updated content
			file.seek(0)
			file.writelines(lines)
		
		# Output success message
		print("<p>Name and email registered successfully!</p>")

	except Exception as e:
		# Output error message
		print(f"<p>Error registering name and email: {str(e)}</p>")
else:
	print("<p>Invalid data submitted.</p>") """
