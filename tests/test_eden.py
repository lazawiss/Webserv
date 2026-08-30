import requests
from termcolor import cprint
import subprocess
import time
import os

SERVER = "http://127.0.0.1:8080"
SERVER2 = "http://127.0.0.1:8081"  # For multi-port test, adapt if needed

def print_ok(msg):
    cprint("✔ " + msg, "green")

def print_fail(msg):
    cprint("✘ " + msg, "red")

def check_status(resp, expected, desc):
    if resp.status_code == expected:
        print_ok(f"{desc}: {resp.status_code}")
    else:
        print_fail(f"{desc}: got {resp.status_code}, expected {expected}")

def test_get_root():
    r = requests.get(SERVER + "/")
    check_status(r, 200, "GET / (homepage)")

def test_get_autoindex():
    r = requests.get(SERVER + "/kiki/")
    check_status(r, 200, "GET /kiki/ (autoindex)")
    if "<ul>" not in r.text:
        print_fail("Autoindex HTML missing <ul>")

def test_404():
    r = requests.get(SERVER + "/doesnotexist")
    check_status(r, 404, "GET /doesnotexist (404)")

def test_405():
    r = requests.post(SERVER + "/kiki/")
    check_status(r, 405, "POST /kiki/ (405)")

def test_403():
    # You need a file or folder with no read permission for this test
    os.system("touch ./www/forbidden && chmod 000 ./www/forbidden")
    try:
        r = requests.get(SERVER + "/forbidden")
        check_status(r, 403, "GET /forbidden (403)")
    finally:
        os.system("chmod 644 ./www/forbidden")

def test_400():
    # Send a malformed request using netcat
    import socket
    s = socket.socket()
    s.connect(("localhost", 8080))
    s.send(b"BADREQUEST\r\n\r\n")
    data = s.recv(1024)
    s.close()
    if b"400" in data:
        print_ok("Malformed request (400)")
    else:
        print_fail("Malformed request (400)")

def test_413():
    # Send a huge POST (adapt /upload and server config if needed)
    big_data = "A" * (10 * 1024 * 1024)  # 10MB
    try:
        r = requests.post(SERVER + "/upload", data=big_data)
        if r.status_code == 413:
            print_ok("POST too large (413)")
        else:
            print_fail(f"POST too large: got {r.status_code}, expected 413")
    except requests.exceptions.ConnectionError:
        print_ok("POST too large (413): connection reset by server (excepted)")

def test_409():
    # Ensure ./www/upload/testfile exists
    upload_path = "./www/upload/testfile"
    with open(upload_path, "w") as f:
        f.write("existing file")
    # Ensure ./testfile exists to upload
    local_file = "./testfile"
    if not os.path.isfile(local_file):
        with open(local_file, "w") as f:
            f.write("new upload")
    files = {'file': open(local_file, 'rb')}
    r = requests.post(SERVER + "/upload", files=files)
    if r.status_code == 409:
        print_ok("POST conflict (409)")
    else:
        print_fail(f"POST conflict: got {r.status_code}, expected 409")

def test_204():
    fname = "./www/upload/testfile2"
    # Only create the file if it doesn't exist
    if not os.path.exists(fname):
        with open(fname, "w") as f:
            f.write("test2")
    r = requests.delete(SERVER + "/upload/testfile2")
    check_status(r, 204, "DELETE /upload/testfile2 (204)")
    # Optionally, ensure the file is gone
    if os.path.exists(fname):
        os.remove(fname)

def test_301():
    # You must configure a redirect in your config for this test
    r = requests.get(SERVER + "/redirect", allow_redirects=False)
    check_status(r, 301, "GET /redirect (301)")
    if "Location" not in r.headers:
        print_fail("301 missing Location header")

def test_500():
    # Simulate internal error (e.g., CGI crash)
    r = requests.get(SERVER + "/cgi-crash")
    check_status(r, 500, "GET /cgi-crash (500)")

def test_501():
    # Use an unimplemented method
    r = requests.request("TRACE", SERVER + "/")
    check_status(r, 501, "TRACE / (501)")

def test_chunked():
    import subprocess
    test_file = "./www/upload/test.txt"
    if os.path.exists(test_file):
        os.remove(test_file)
    # Use netcat to send the raw HTTP request from chunked-request.txt
    with open("./examples-txt/chunked-request.txt", "rb") as f:
        proc = subprocess.Popen(
            ["nc", "localhost", "8080"],
            stdin=f,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        out, err = proc.communicate(timeout=3)
        # Look for HTTP status code in the response
        if b"201" in out:
            print_ok("POST chunked (201)")
        else:
            print_fail(f"POST chunked: got response:\n{out.decode(errors='ignore')}\nexpected 201")
    # Cleanup: remove the file created by the CGI
    if os.path.exists(test_file):
        os.remove(test_file)

import signal

def test_multiport():
    # Test second port (adapt if needed)
    r = requests.get(SERVER2 + "/")
    check_status(r, 200, "GET / on second port (multiport)")

def test_timeout_on_incomplete_request():
    import socket
    import time

    s = socket.socket()
    s.connect(("localhost", 8080))
    # Send only the beginning of a POST request, then sleep
    s.send(b"POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1000000\r\n\r\nA" * 10)
    time.sleep(10)  # Sleep longer than your server's timeout (e.g., 10s)
    try:
        data = s.recv(1024)
        if not data:
            print_ok("Timeout on incomplete request: connection closed by server")
        else:
            print_fail("Timeout on incomplete request: server did not close connection")
    except Exception as e:
        print_ok("Timeout on incomplete request: connection closed by server")
    s.close()

def main():
    print("Starting webserv tests...\n")
    test_get_root()
    test_get_autoindex()
    test_404()
    test_405()
    test_403()
    test_400()
    test_413()
    test_409()
    test_204()
    test_301()
    test_501()
    test_chunked()
    test_multiport()
    test_timeout_on_incomplete_request()
    print("\nAll HTTP tests done.\n")

if __name__ == "__main__":
    main()