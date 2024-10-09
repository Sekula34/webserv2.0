#!/usr/bin/env python3

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
print(f"<p>Current Working Directory: {os.getcwd()}</p>")





# #!/usr/bin/env python3

# import cgi
# import cgitb
# import html

# cgitb.enable()  # Enable debugging

# # Set the content-type header for HTTP response
# print("Content-Type: text/html\n")

# # Parse the form data for both GET and POST
# form = cgi.FieldStorage()
# name = form.getvalue("name")
# email = form.getvalue("email")

# # Safely escape the input to avoid HTML injection
# if name and email:
# 	safe_name = html.escape(name)
# 	safe_email = html.escape(email)

# 	# File path to the database HTML file
# 	# database_file = "path/to/your/database.html"  # Adjust the path to where your database.html is located
# 	database_file = "../../www/basic-website/public/pages/database.html"  # Adjust the path to where your database.html is located

# 	# Check if the script can access the file
# 	if not os.access(database_file, os.R_OK | os.W_OK):
# 		print(f"<p>Error: Script does not have read/write access to {database_file}</p>")
# 	else:
# 		print(f"<p>Script has read/write access to {database_file}</p>")

# 	try:
# 		# Append the new name to the HTML file inside the <ul> element
# 		with open(database_file, "r+") as file:
# 			lines = file.readlines()
			
# 			# Find the line where the <ul id="namesList"> tag is located
# 			for i, line in enumerate(lines):
# 				if '<ul id="namesList">' in line:
# 					# Insert the new name just after the <ul> opening tag
# 					lines.insert(i + 1, f"        <li>Name: {safe_name}, Email: {safe_email}</li>\n")
# 					break
			
# 			# Move the file pointer to the beginning and write updated content
# 			file.seek(0)
# 			file.writelines(lines)
		
# 		# Output success message
# 		print("<p>Name and email registered successfully!</p>")

# 	except Exception as e:
# 		# Output error message
# 		print(f"<p>Error registering name and email: {str(e)}</p>")
# else:
# 	print("<p>Invalid data submitted.</p>")
