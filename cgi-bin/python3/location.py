#!/usr/bin/env python3

import os

# Function to dynamically build paths based on the server's document root
def build_path(relative_path):
	# Constructs a full path based on the server's root directory.
	# If I want a fallback document, I can use: root_dir = os.environ.get('DOCUMENT_ROOT', '../../www/basic-website/public')
	# relative_prefix = "../../" # Path traversal so the script can access www folder.
	root_dir = os.environ['DOCUMENT_ROOT']
	return os.path.join(root_dir, relative_path)
	# return os.path.join(relative_prefix, root_dir, relative_path)

# resourse = build_path('pages/about.html')
# resource = "pages/about.html"

server = os.environ['SERVER_NAME']
port = os.environ['PORT']

print("Status: 302 Found")
print(f"Location: http://{server}:{port}/pages/about.html")
print("")

# print(f"Location: {resource}")
# print("Location: http://localhost:9494/pages/about.html")
# print("Location: ../../www/basic-website/public/pages/about.html")