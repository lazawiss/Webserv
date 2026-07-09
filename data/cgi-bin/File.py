#!C:\Python311\python.exe

import os
import sys
from urllib.parse import parse_qs

method = os.environ.get("REQUEST_METHOD", "GET")

if method == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
else:
    body = os.environ.get("QUERY_STRING", "")

form = parse_qs(body)
print(f"DEBUG method={method} body={repr(body)} form={form}", file=sys.stderr)

username = form.get("username", [""])[0]
emailaddress = form.get("emailaddress", [""])[0]

print("Content-type:text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title> List </title>")
print("</head>")
print("<body>")
print("<h3> List of names & emails</h3>")
print(username)
print(emailaddress)
print("</body>")
print("</html>")