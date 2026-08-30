#!/usr/bin/env python3
import sys, os
from urllib.parse import parse_qsl

print(repr(os.environ.get("CONTENT_LENGTH")))
content_length = int(os.environ.get("CONTENT_LENGTH", 0))
body = b""
while len(body) < content_length:
    chunk = sys.stdin.buffer.read(content_length - len(body))
    if not chunk:
        break
    body += chunk

# parse_qsl gives you [('username', 'hello'), ('emailaddress', 'helo@fresh.fr')]
# already percent-decoded — %40 becomes @ for you
pairs = parse_qsl(body.decode("utf-8"))

upload_path = "data/upload/uploaded_file"
with open(upload_path, "w") as f:
    for key, value in pairs:
        f.write(value + ",")

print("Content-type:text/html\r\n\r\n")
print()
print("<html>")
print("<head>")
print("<title> List </title>")
print("</head>")
print("<body>")
print(f"Saved {len(body)} fields to {upload_path}")
print("</body>")
print("</html>")

