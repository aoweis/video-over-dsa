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

framerate=25
filesink_location="video1.ts"

#Name without .c extension
cprog_name= "working"


command= "./"+str(cprog_name)+" "+filesink_location;

print(command)
os.system(command)

