# Never reads stdin. On a POST the body pipe fills and stdin fd stays registered, so CGI sits in _fdToCGI under BOTH fds when the
# check
import time
time.sleep(300)
