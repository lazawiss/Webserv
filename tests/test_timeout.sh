#!/usr/bin/env bash

set -u

URL="${1:-http://localhost/cgi-bin/timeout_test.py?delay=30}"
TIMEOUT="${2:-10}"

echo "========================================"
echo "CGI timeout test"
echo "========================================"
echo "URL:     $URL"
echo "Timeout: ${TIMEOUT}s"
echo

start=$(date +%s)

HTTP_CODE=$(curl \
    --silent \
    --show-error \
    --max-time "$TIMEOUT" \
    --output /tmp/cgi_timeout_test.out \
    --write-out "%{http_code}" \
    "$URL" 2>/tmp/cgi_timeout_test.err)

CURL_EXIT=$?

end=$(date +%s)
elapsed=$((end - start))

echo "Elapsed: ${elapsed}s"
echo "curl exit code: $CURL_EXIT"
echo "HTTP status: $HTTP_CODE"
echo

if [[ "$CURL_EXIT" -eq 0 ]]; then
    echo "RESULT: CGI completed before the client timeout."
    echo
    echo "Response:"
    cat /tmp/cgi_timeout_test.out

elif [[ "$CURL_EXIT" -eq 28 ]]; then
    echo "RESULT: CLIENT TIMEOUT."
    echo "curl stopped waiting after ${TIMEOUT}s."
    echo
    echo "This means the CGI did not complete within the client timeout."

else
    echo "RESULT: REQUEST FAILED."
    echo
    cat /tmp/cgi_timeout_test.err
fi

echo
echo "========================================"