#!/usr/bin/env python3
import os
import json
import sys

# Path to the log file
log_file = "/tmp/cgi_debug.log"

# Function to log messages to a file
def log_message(message):
	with open(log_file, "a") as log:
		log.write(f"{message}\n")

log_message("list_file.py script started")

# Log environment variables for debugging
log_message(f"CONTENT_TYPE: {os.environ.get('CONTENT_TYPE')}")
log_message(f"REQUEST_METHOD: {os.environ.get('REQUEST_METHOD')}")
log_message(f"QUERY_STRING: {os.environ.get('QUERY_STRING')}")

# Print the Content-Type header for HTTP response
print("Content-Type: application/json\n")

# Log and list files in the upload directory
upload_dir = "../../www/basic-website/public/uploads"
try:
	files = os.listdir(upload_dir)
	log_message(f"Files in '{upload_dir}': {files}")
	json_output = json.dumps(files)
	log_message(f"Generated JSON: {json_output}")
	print(json_output)
except Exception as e:
	error_message = f"Error reading directory '{upload_dir}': {e}"
	log_message(error_message)
	print(json.dumps(files))
	sys.stdout.flush()

# ==========================================================================

""" #!/usr/bin/env python3
import os
import json

print("Content-Type: application/json\n")
print("")
# print("Current Working Directory:", os.getcwd())  # Print the current working directory
upload_dir = "../../www/basic-website/public/uploads"
files = os.listdir(upload_dir)
print(json.dumps(files))  # Return the list of filenames as JSON """

# # Construct the absolute path to the uploads directory
# script_dir = os.path.dirname(__file__)  # Directory where the script is located
# upload_dir = os.path.join(script_dir, "../public/uploads")  # Adjust relative to the script's location

# # Use the absolute path to list files
# upload_dir = os.path.abspath(upload_dir)  # Convert to an absolute path