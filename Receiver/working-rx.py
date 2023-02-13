import os
import sys
import math
import struct
import threading
from datetime import datetime
import time

class ThreadClass(threading.Thread):
    def run(self):
        return

filesource_location="/home/ahmad/GitHub/video-over-dsa/Transmitter/video1.ts"

cprog_name= "working-rx"


command= "./" + str(cprog_name) + " " + filesource_location

print(command)
os.system(command)

