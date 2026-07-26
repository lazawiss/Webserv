#!/bin/bash
# ============================================================================
# HTTP 206 Partial Content (byte-range) test suite for Webserv
# ----------------------------------------------------------------------------
# Linux/Docker only (Webserv uses epoll). Run from repo root:
#
#     make
#     ./webserv ./data/config/configauto &
#     ./tests/test_range.sh
#
# The 206 path is wired into the image branch of handleRequest(), so we test
# against a file served through /images/. Adjust IMG_URL if your routing
# maps images differently.
# ============================================================================

HOST="127.0.0.1"; PORT="8080"; BASE="http://$HOST:$PORT"
IMG_NAME="range_test.png"
IMG_URL="$BASE/images/$IMG_NAME"

PASS=0; FAIL=0
ok()   { echo "  PASS  $1"; PASS=$((PASS+1)); }
bad()  { echo "  FAIL  $1"; FAIL=$((FAIL+1)); }

# Create a deterministic 1000-byte file the server can serve via /images/.
mkdir -p data/www/images
python3 - "$IMG_NAME" <<'PY' 2>/dev/null || perl -e 'print "A"x1000' > "data/www/images/range_test.png"
import sys
open("data/www/images/"+sys.argv[1],"wb").write(bytes((i%256) for i in range(1000)))
PY
TOTAL=$(wc -c < data/www/images/$IMG_NAME | tr -dc '0-9')
echo "=== 206 tests against $IMG_URL (size=$TOTAL) ==="

# 1. Explicit range bytes=0-99 -> 206, Content-Length 100, correct Content-Range.
HDR=$(curl -s -D - -o /dev/null -H "Range: bytes=0-99" "$IMG_URL")
echo "$HDR" | grep -qi "206 Partial Content"                 && ok "bytes=0-99 -> 206"                 || bad "bytes=0-99 -> 206"
echo "$HDR" | grep -qi "^Content-Length: *100"               && ok "Content-Length is slice (100)"     || bad "Content-Length is slice (100)"
echo "$HDR" | grep -qi "^Content-Range: *bytes 0-99/$TOTAL"  && ok "Content-Range 0-99/$TOTAL"          || bad "Content-Range 0-99/$TOTAL"
echo "$HDR" | grep -qi "^Accept-Ranges: *bytes"              && ok "Accept-Ranges: bytes present"      || bad "Accept-Ranges: bytes present"

# 2. Body of the range is exactly 100 bytes.
LEN=$(curl -s -H "Range: bytes=0-99" "$IMG_URL" | wc -c | tr -dc '0-9')
[ "$LEN" = "100" ] && ok "returned body is 100 bytes" || bad "returned body is $LEN (want 100)"

# 3. Suffix range bytes=-50 -> last 50 bytes.
HDR=$(curl -s -D - -o /dev/null -H "Range: bytes=-50" "$IMG_URL")
echo "$HDR" | grep -qi "^Content-Range: *bytes $((TOTAL-50))-$((TOTAL-1))/$TOTAL" && ok "suffix bytes=-50" || bad "suffix bytes=-50"

# 4. Open-ended range bytes=900- -> to EOF.
HDR=$(curl -s -D - -o /dev/null -H "Range: bytes=900-" "$IMG_URL")
echo "$HDR" | grep -qi "^Content-Range: *bytes 900-$((TOTAL-1))/$TOTAL" && ok "open-ended bytes=900-" || bad "open-ended bytes=900-"

# 5. Unsatisfiable range (start past EOF) -> 416.
CODE=$(curl -s -o /dev/null -w "%{http_code}" -H "Range: bytes=99999-100000" "$IMG_URL")
[ "$CODE" = "416" ] && ok "out-of-bounds -> 416" || bad "out-of-bounds -> got $CODE (want 416)"

# 6. No Range header -> normal 200 full file.
CODE=$(curl -s -o /dev/null -w "%{http_code}" "$IMG_URL")
[ "$CODE" = "200" ] && ok "no Range -> 200" || bad "no Range -> got $CODE (want 200)"

echo
echo "=== $PASS passed, $FAIL failed ==="
[ "$FAIL" -eq 0 ]
