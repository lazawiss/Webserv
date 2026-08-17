#!/usr/bin/env python3

import os
import sys
import time
import urllib.parse

# Default delay: 30 seconds
default_delay = 30

# Allow: /cgi-bin/timeout_test.py?delay=60
query = urllib.parse.parse_qs(os.environ.get("QUERY_STRING", ""))
try:
    delay = float(query.get("delay", [default_delay])[0])
except (ValueError, TypeError):
    delay = default_delay

# Optional logging to stderr
print(f"Sleeping for {delay} seconds...", file=sys.stderr, flush=True)

time.sleep(delay)

# CGI response
print("Content-Type: text/plain")
print()
print(f"CGI completed successfully after {delay} seconds.")
sys.stdout.flush()