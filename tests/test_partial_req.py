# TEST 1 : HEADERS WITHOUT TERMINATING BLANK LINE (NO RESPONSE)
python3 -c "
import socket,time
s=socket.socket(); s.connect(('localhost',8080))
s.sendall(b'GET / HTTP/1.1\r\nHost: localhost\r\n')
s.settimeout(3)
try: print('RESPONSE:', s.recv(4096).decode(errors='replace'))
except socket.timeout: print('NO RESPONSE — correct, server waiting for more')
"