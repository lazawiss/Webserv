#!/usr/bin/env python3
# CGI script: stores names + emails submitted via a form into a small
# CSV "database" and lets you view the current contents.
#   GET  /cgi-bin/database.py   -> shows the form + all stored contacts
#   POST /cgi-bin/database.py   -> adds one contact, then shows the list
#
# The POST body is a urlencoded form (username=...&emailaddress=...), the same
# format the existing post.html form sends.

import sys
import os
import csv
import html
from urllib.parse import parse_qsl
from datetime import datetime

DB_DIR = "data/database"
DB_PATH = os.path.join(DB_DIR, "contacts.csv")
FIELDS = ["timestamp", "name", "email"]


def respond(body, status="200 OK"):
    """Write a CGI response: header block, blank line, then the body.
    The server (buildResponse) reads Status/Content-Type and adds
    Content-Length itself."""
    sys.stdout.write("Status: %s\r\n" % status)
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
    sys.stdout.write("\r\n")
    sys.stdout.write(body)
    sys.stdout.flush()


def read_body():
    """Read exactly CONTENT_LENGTH bytes from stdin (the request body)."""
    length = int(os.environ.get("CONTENT_LENGTH") or 0)
    data = b""
    while len(data) < length:
        chunk = sys.stdin.buffer.read(length - len(data))
        if not chunk:
            break
        data += chunk
    return data.decode("utf-8", errors="replace")


def load_records():
    """Return every stored contact as a list of dicts."""
    records = []
    if os.path.exists(DB_PATH):
        with open(DB_PATH, newline="") as f:
            for row in csv.DictReader(f):
                records.append(row)
    return records


def save_record(name, email):
    """Append one contact to the CSV, writing a header row if the file is new."""
    os.makedirs(DB_DIR, exist_ok=True)
    is_new = not os.path.exists(DB_PATH)
    with open(DB_PATH, "a", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS)
        if is_new:
            writer.writeheader()
        writer.writerow({
            "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "name": name,
            "email": email,
        })


def render_page(records, message=""):
    """Build the HTML: an optional status message, the add form, and a table.
    Every value is html.escape()d so a malicious name/email can't inject markup."""
    rows = ""
    for r in records:
        rows += "<tr><td>%s</td><td>%s</td><td>%s</td></tr>" % (
            html.escape(r.get("timestamp", "")),
            html.escape(r.get("name", "")),
            html.escape(r.get("email", "")),
        )
    if not rows:
        rows = "<tr><td colspan='3'><em>No contacts yet.</em></td></tr>"

    return """<!DOCTYPE html>
<html>
<head><meta charset="utf-8"><title>Contact database</title></head>
<body>
  <h1>Contact database</h1>
  %s
#   <form method="post">
    Name: <input name="username" required />
    Email: <input type="email" name="emailaddress" required />
    <button type="submit">Add</button>
  </form>
  <table border="1" cellpadding="6" style="margin-top:1em;border-collapse:collapse">
    <tr><th>Added</th><th>Name</th><th>Email</th></tr>
    %s
  </table>
</body>
</html>""" % (message, rows)


def main():
    method = os.environ.get("REQUEST_METHOD", "GET").upper()

    if method == "POST":
        fields = dict(parse_qsl(read_body()))
        name = (fields.get("nom") or fields.get("name") or "").strip()
        email = (fields.get("emailaddress") or fields.get("email") or "").strip()

        # basic validation
        if not name or not email or "@" not in email:
            respond(
                render_page(
                    load_records(),
                    "<p style='color:red'>A name and a valid email are required.</p>",
                ),
                status="400 Bad Request",
            )
            return

        # avoid storing the same email twice
        existing = load_records()
        if any(r.get("email", "").lower() == email.lower() for r in existing):
            respond(render_page(
                existing,
                "<p style='color:darkorange'>%s is already in the database.</p>"
                % html.escape(email),
            ))
            return

        save_record(name, email)
        respond(render_page(
            load_records(),
            "<p style='color:green'>Saved %s &lt;%s&gt;.</p>"
            % (html.escape(name), html.escape(email)),
        ))
        return

    # GET (or anything else): just display the current database
    respond(render_page(load_records()))


if __name__ == "__main__":
    main()
