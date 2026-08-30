#!/bin/bash
exec 3<>/dev/tcp/127.0.0.1/8080
printf "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n" >&3
# note: no final \r\n\r\n — request is incomplete on purpose
sleep 15   # longer than your configured timeout
cat <&3    # see if the server closed the connection
exec 3<&-  # close the fd