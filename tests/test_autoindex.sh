#!/bin/bash
# ============================================================================
# Autoindex test suite for Webserv
# ----------------------------------------------------------------------------
# NOTE: Webserv uses epoll -> it only builds/runs on Linux (or a Linux Docker
#       container). Run this from the repo root on a Linux box:
#
#         make
#         ./webserv ./data/config/configauto &
#         SERVER_PID=$!
#         ./tests/test_autoindex.sh
#         kill $SERVER_PID
#
# HOST/PORT come from data/config/configauto (127.0.0.1:8080).
# ============================================================================

HOST="127.0.0.1"
PORT="8080"
BASE="http://$HOST:$PORT"

PASS=0
FAIL=0

# ----------------------------------------------------------------------------
# check <description> <expected-substring-or-status> <curl-output>
# ----------------------------------------------------------------------------
expect_status() {
    local desc="$1"; local want="$2"; local url="$3"
    local got
    got=$(curl -s -o /dev/null -w "%{http_code}" "$url")
    if [ "$got" = "$want" ]; then
        echo "  PASS  [$got] $desc"
        PASS=$((PASS+1))
    else
        echo "  FAIL  [got $got, want $want] $desc  ($url)"
        FAIL=$((FAIL+1))
    fi
}

expect_body_contains() {
    local desc="$1"; local needle="$2"; local url="$3"
    local body
    body=$(curl -s "$url")
    if echo "$body" | grep -q -- "$needle"; then
        echo "  PASS  $desc (found '$needle')"
        PASS=$((PASS+1))
    else
        echo "  FAIL  $desc (missing '$needle')  ($url)"
        FAIL=$((FAIL+1))
    fi
}

expect_header_contains() {
    local desc="$1"; local needle="$2"; local url="$3"
    local hdrs
    hdrs=$(curl -s -D - -o /dev/null "$url")
    if echo "$hdrs" | grep -qi -- "$needle"; then
        echo "  PASS  $desc (header '$needle')"
        PASS=$((PASS+1))
    else
        echo "  FAIL  $desc (missing header '$needle')  ($url)"
        echo "$hdrs" | sed 's/^/          /'
        FAIL=$((FAIL+1))
    fi
}

echo "=== Autoindex tests against $BASE ==="

# 1. A directory with autoindex ON and no index.html -> 200 + listing.
#    data/www/html is the location "/" root in configauto (autoindex on).
#    Make sure there is a sub-directory without index.html to list.
mkdir -p data/www/html/listme
: > data/www/html/listme/alpha.txt
: > data/www/html/listme/beta.txt

expect_status        "GET /listme/ returns 200 (autoindex on)"        "200" "$BASE/listme/"
expect_header_contains "listing is served as HTML"                    "Content-Type: text/html" "$BASE/listme/"
expect_body_contains "listing contains entry alpha.txt"               "alpha.txt" "$BASE/listme/"
expect_body_contains "listing contains entry beta.txt"                "beta.txt"  "$BASE/listme/"
expect_body_contains "listing is a real HTML index page"              "Index of"  "$BASE/listme/"

# 2. A directory that DOES contain index.html -> serve index.html, not a listing.
mkdir -p data/www/html/hasindex
echo "<h1>REAL INDEX</h1>" > data/www/html/hasindex/index.html
expect_body_contains "index.html wins over autoindex"                 "REAL INDEX" "$BASE/hasindex/"

# 3. autoindex OFF on /api -> a directory with no index must be 403 (not a listing).
expect_status        "GET /api/ with autoindex off -> 403"            "403" "$BASE/api/"

# 4. Non-existent directory -> 404.
expect_status        "GET /nope/ -> 404"                              "404" "$BASE/nope/"

# 5. Content-Length must match the body length (regression guard: header said 0).
LEN_HEADER=$(curl -s -D - -o /dev/null "$BASE/listme/" | grep -i '^Content-Length:' | tr -dc '0-9')
LEN_BODY=$(curl -s "$BASE/listme/" | wc -c | tr -dc '0-9')
if [ -n "$LEN_HEADER" ] && [ "$LEN_HEADER" = "$LEN_BODY" ]; then
    echo "  PASS  Content-Length ($LEN_HEADER) matches body length"
    PASS=$((PASS+1))
else
    echo "  FAIL  Content-Length ($LEN_HEADER) != body length ($LEN_BODY)"
    FAIL=$((FAIL+1))
fi

echo
echo "=== $PASS passed, $FAIL failed ==="
[ "$FAIL" -eq 0 ]
