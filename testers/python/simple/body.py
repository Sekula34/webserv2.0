import http.client

# Define the connection
conn = http.client.HTTPConnection("localhost", 9090)

# Headers for the POST request
headers = {
    'Content-type': 'text/plain',
}

# Body of the POST request
body = "sending a body to cgi finally works!"

# Sending the POST request
conn.request("POST", "/cgi-bin/io_test.py", body, headers)  # Adjust the path as needed

# Getting the response
response = conn.getresponse()

# Print the response status and body
print(response.status, response.reason)
print(response.read().decode())

# Close the connection
conn.close()

