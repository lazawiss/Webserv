# Slow mais OK in TIMEOUT
import sys, time
time.sleep(3)
sys.stdout.write("Content-Type: text/plain\r\n\r\n")
sys.stdout.write("slow but finished\n")
