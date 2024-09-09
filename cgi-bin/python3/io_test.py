
import os
import sys
start = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>checking CGI meta variables</title>
            <style>
        body {
            background-color: #121212;
            color: #e0e0e0;
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            text-align: center;
            height: 100%;
            margin: 30px;
            padding: 30px;
            font-size: 20px;
        }
        h1, h2, h3 {
            margin: 15px;
        }
        h1 {
            font-size: 2.5em;
        }
        h2 {
            font-size: 2em;
        }
        h3 {
            font-size: 1.75em;
        }
        p {
            font-size: 0.75em;
            background-color: #1e1e1e;
            padding: 10px;
            margin: 5px 0;
            border-radius: 8px;
            max-width: 600px;
            line-height: 1.5;
        }
    </style>
</head>
<body>
"""
end = """
  </body>
</html>
"""

print("HTTP/1.1 200 OK\n")
print("Connection: close\n")
print("Content-Language: en\n")
print("Content-Length: 1000\n")
print("\n\n")

print(start)


# Print environment variables
print("<h3>Environment Variables:</h3>")
for key, value in os.environ.items():
    print(f"<p>{key}={value}</p>")

print("\n<p>Received from stdin:</p>")

# Read and print from stdin
for line in sys.stdin:
    print(f"<p>{line}</p>")
# first_line = sys.stdin.readline()
# print(f"<p>{first_line}</p>")

print (end)
