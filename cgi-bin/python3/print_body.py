
import os
import sys

print("Status: 200 OK", end="\r\n")
print("Content-Type: text/html", end="\r\n")
print("Language: English", end="\r\n\r\n")

print("\n<p>Received from stdin:</p>")

# Read and print from stdin
for line in sys.stdin:
    print(f"<p>{line}</p>")

