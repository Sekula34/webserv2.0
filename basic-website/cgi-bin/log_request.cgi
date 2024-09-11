#!/usr/bin/env python3
import cgi
import os
import sys

# List of environment variables to exclude from printing
excluded_keys = [
	"MAIL", "LANGUAGE", "USER", "XDG_SESSION_TYPE", "SHLVL", "HOME", "OLDPWD",
	"LESS", "DESKTOP_SESSION", "ZSH", "LSCOLORS", "GTK_MODULES", "XDG_SEAT_PATH",
	"PAGER", "SYSTEMD_EXEC_PID", "DBUS_SESSION_BUS_ADDRESS", "DBUS_STARTER_BUS_TYPE",
	 "LIBVIRT_DEFAULT_URI", "COLORTERM", "GTK_IM_MODULE", "LOGNAME", "_", "XDG_SESSION_CLASS",
	 "TERM", "FT_HOOK_PATHNAME", "RUSTUP_HOME", "GNOME_DESKTOP_SESSION_ID", "PATH",
	"SESSION_MANAGER", "GDM_LANG", "GNOME_TERMINAL_SCREEN", "XDG_MENU_PREFIX",
	"XDG_SESSION_PATH", "XDG_RUNTIME_DIR", "DISPLAY", "LANG", "XDG_CURRENT_DESKTOP",
	"XDG_SESSION_DESKTOP", "GNOME_TERMINAL_SERVICE", "XMODIFIERS", "XAUTHORITY",
	"LS_COLORS", "SSH_AUTH_SOCK", "XDG_GREETER_DATA_DIR", "SSH_AGENT_LAUNCHER",
	"KRB5CCNAME", "SHELL", "QT_ACCESSIBILITY", "GDMSESSION", "FT_HOOK_NAME",
	"DOCKER_HOST", "GPG_AGENT_INFO", "QT_IM_MODULE", "PWD", "XDG_CONFIG_DIRS",
	"XDG_DATA_DIRS", "DBUS_STARTER_ADDRESS", "CARGO_HOME", "VTE_VERSION",
	"CHROME_DESKTOP", "ORIGINAL_XDG_CURRENT_DESKTOP", "GDK_BACKEND", "TERM_PROGRAM",
	"TERM_PROGRAM_VERSION", "GIT_ASKPASS", "VSCODE_GIT_ASKPASS_NODE",
	"VSCODE_GIT_ASKPASS_EXTRA_ARGS", "VSCODE_GIT_ASKPASS_MAIN", "VSCODE_GIT_IPC_HANDLE",
	"VSCODE_INJECTION", "ZDOTDIR",  "USER_ZDOTDIR",
]

# Print the content-type header for HTTP response
print("Content-Type: text/html\n")

# Function to print all environment variables
def print_environment_variables():
	print("<h2>General Environment Variables</h2>")
	print("<pre>")
	for key, value in os.environ.items():
		print(f"{key}={value}")
	print("</pre>")

# Function to print webserver-related environment variables, excluding specified keys
def print_webserver_environment_variables():
	print("<h2>Webserver-Related Environment Variables</h2>")
	print("<pre>")
	for key, value in os.environ.items():
		# Print only variables not in the excluded_keys list
		if key not in excluded_keys:
			print(f"{key}={value}")
	print("</pre>")

# Function to print request headers and body
def print_request_details():
	print("<h2>Request Headers and Body</h2>")
	print("<pre>")

	# Print headers from environment variables
	print("Headers:")
	headers = {key: value for key, value in os.environ.items() if key.startswith("HTTP_")}
	for key, value in headers.items():
		print(f"{key}={value}")

	# Handle and print parsed form data
	print("\nParsed Body:")
	form = cgi.FieldStorage()
	for field in form.keys():
		item = form[field]
		if item.filename:
			print(f"File uploaded: {item.filename}")
		else:
			print(f"{field}: {form.getvalue(field)}")

	# Print raw body
	# print("\nRaw Body:")
	# # Read the body only if CONTENT_LENGTH is set and is a valid integer
	# content_length = os.environ.get('CONTENT_LENGTH')
	# # Initialize raw_body variable to ensure it doesn't attempt reading stdin by default
	# raw_body = ''
	# if content_length and content_length.isdigit() and int(content_length) > 0:
	# 	try:
	# 		# Read only the specified length of input
	# 		raw_body = sys.stdin.read(int(content_length))
	# 		print(raw_body.encode('unicode_escape').decode('utf-8'))  # Display with escape sequences
	# 	except Exception as e:
	# 		print(f"Error reading raw body: {e}")
	# else:
	# 	print("No valid CONTENT_LENGTH or zero length; skipping raw body read.")

	print("</pre>")

# Main script logic
print("<html><body>")
print("<h1>CGI Request Logger</h1>")

# Print all environment variables
print_environment_variables()

# Print webserver environment variables
print_webserver_environment_variables()

# Print request headers and body
print_request_details()

print("</body></html>")
