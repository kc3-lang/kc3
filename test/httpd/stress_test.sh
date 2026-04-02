#!/bin/sh
## Stress test for KC3 HTTPd with EPoll + ASan
## Usage: cd test/httpd && ./stress_test.sh
## Requires: vegeta in PATH or VEGETA env var set

set -e

export ASAN_OPTIONS="detect_odr_violation=0"

SRC_TOP="../.."
VEGETA="${VEGETA:-/home/tokyovania/Downloads/vegeta_12.13.0_linux_amd64/vegeta}"
HOST="127.0.0.1"
PORT="15004"
HTTPD_ASAN="${SRC_TOP}/httpd/kc3_httpd_asan"
ASAN_LOG="stress_asan.log"
VEGETA_RESULTS="stress_vegeta_results.txt"
TARGETS="vegeta_targets.txt"

if [ ! -x "$VEGETA" ]; then
    echo "ERROR: vegeta not found at $VEGETA"
    echo "Set VEGETA env var to the vegeta binary path"
    exit 1
fi

if [ ! -x "$HTTPD_ASAN" ]; then
    echo "ERROR: kc3_httpd_asan not found. Run 'make asan' from project root."
    exit 1
fi

echo "=== KC3 EPoll Stress Test ==="
echo ""

echo "[1/6] Cleaning dumps and cache..."
rm -f kc3.dump db/*.bin.facts
find . -name '*.kc3c' -delete

echo "[2/6] Creating dump (first run)..."
rsync -a --delete ../../doc ./
if [ -d ../../release ]; then
    rsync -aP ../../release/* static/release/ 2>/dev/null || true
fi
SRC_TOP_ABS="$(cd "${SRC_TOP}" && pwd)"
(cd "${SRC_TOP_ABS}" && . ./env && export LD_LIBRARY_PATH) > /dev/null 2>&1
export LD_LIBRARY_PATH="${SRC_TOP_ABS}/libkc3:${SRC_TOP_ABS}/epoll:${SRC_TOP_ABS}/ekc3:${SRC_TOP_ABS}/http:${SRC_TOP_ABS}/markdown:${SRC_TOP_ABS}/socket:${SRC_TOP_ABS}/tls:${SRC_TOP_ABS}/gl:${SRC_TOP_ABS}/image:${SRC_TOP_ABS}/window:${SRC_TOP_ABS}/window/demo:${SRC_TOP_ABS}/window/cairo:${SRC_TOP_ABS}/window/cairo/demo:${SRC_TOP_ABS}/window/cairo/xcb:${SRC_TOP_ABS}/window/egl:${SRC_TOP_ABS}/window/egl/demo:${SRC_TOP_ABS}/window/egl/xcb:${SRC_TOP_ABS}/window/egl/drm:${SRC_TOP_ABS}/window/sdl2:${LD_LIBRARY_PATH}"
"$HTTPD_ASAN" --trace -d "$HOST" "$PORT" 2>&1 | tee dump_asan.log
echo "Dump created."

echo "[3/6] Starting ASan httpd..."
"$HTTPD_ASAN" --trace -d "$HOST" "$PORT" > "$ASAN_LOG" 2>&1 &
HTTPD_PID=$!
echo "PID: $HTTPD_PID"

echo "Waiting for server to listen..."
TRIES=0
while ! curl -sf "http://${HOST}:${PORT}/" > /dev/null 2>&1; do
    TRIES=$((TRIES + 1))
    if [ $TRIES -gt 60 ]; then
        echo "ERROR: Server did not start within 60 seconds"
        kill "$HTTPD_PID" 2>/dev/null
        wait "$HTTPD_PID" 2>/dev/null
        cat "$ASAN_LOG"
        exit 1
    fi
    sleep 1
done
echo "Server is listening on ${HOST}:${PORT}"

echo ""
echo "[4/6] Running vegeta load tests..."
echo "" > "$VEGETA_RESULTS"

for RATE in 10 50 100 200 500 1000; do
    DURATION="10s"
    echo "--- Rate: ${RATE} req/s for ${DURATION} ---"
    "$VEGETA" attack \
        -targets "$TARGETS" \
        -rate "$RATE" \
        -duration "$DURATION" \
        -timeout 5s \
        -workers 10 \
        | "$VEGETA" report >> "$VEGETA_RESULTS" 2>&1
    echo "Rate ${RATE}: done"
    echo "" >> "$VEGETA_RESULTS"
    sleep 2
done

echo ""
echo "[5/6] Stopping server (SIGTERM for ASan report)..."
kill "$HTTPD_PID" 2>/dev/null
wait "$HTTPD_PID" 2>/dev/null || true

echo ""
echo "[6/6] Results"
echo ""
echo "=== VEGETA RESULTS ==="
cat "$VEGETA_RESULTS"

echo ""
echo "=== ASAN REPORT ==="
if grep -q "ERROR: AddressSanitizer" "$ASAN_LOG"; then
    echo "!!! MEMORY ERRORS DETECTED !!!"
    grep -A 20 "ERROR: AddressSanitizer" "$ASAN_LOG"
elif grep -q "LeakSanitizer" "$ASAN_LOG"; then
    echo "!!! MEMORY LEAKS DETECTED !!!"
    grep -A 20 "LeakSanitizer" "$ASAN_LOG"
else
    echo "No ASan errors detected."
fi

echo ""
echo "Full ASan log: $ASAN_LOG"
echo "Full vegeta results: $VEGETA_RESULTS"
echo ""

if grep -q "Segmentation fault\|SIGABRT\|SIGSEGV" "$ASAN_LOG"; then
    echo "!!! SERVER CRASHED !!!"
    exit 1
fi

echo "=== STRESS TEST COMPLETE ==="
