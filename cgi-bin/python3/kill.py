# endless_loop.py

import time  # Import the time module
import signal

# Define a signal handler function that does nothing
def ignore_sigterm(signum, frame):
    print(f"Ignored SIGTERM signal: {signum}")

# Set the handler for the SIGTERM signal to the ignore function
signal.signal(signal.SIGTERM, ignore_sigterm)

while True:
    print("<p>This loop will run forever.</p>")
    time.sleep(1)  # Wait for 0.5 seconds before the next iteration

