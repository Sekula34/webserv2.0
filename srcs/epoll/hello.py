
import os
import sys

print("hello world from CGI!")

# Print environment variables
print("Environment Variables:")
for key, value in os.environ.items():
    print(f"{key}: {value}")

print("\nReceived from stdin:")

# Read and print from stdin
for line in sys.stdin:
    print(line, end='')


