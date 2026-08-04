#!/bin/bash
for i in {1..20}; do
    (printf "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n"; sleep 1) | nc 127.0.0.1 8080 &
done
wait