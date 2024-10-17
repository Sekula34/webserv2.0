import time  # Import the time module

print("Status: 200 OK", end="\r\n")
print("Content-Type: text/html", end="\r\n")
print("Content-Length: 18", end="\r\n")
print("Language: English", end="\r\n\r\n")

print("<p>hello world</p>")

time.sleep(10)  # Wait for 0.5 seconds before the next iteration
