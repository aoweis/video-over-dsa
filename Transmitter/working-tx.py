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

# To get a list of available video drivers, use: $ ls /dev/video*
# Provide the video driver name of the input camera to the variable usb_driver_name

filesink_location="video1.ts"
usb_driver_name = "v4l2src"


cprog_name= "working-tx"


command= "./" + str(cprog_name) + " " + filesink_location + " " + usb_driver_name;

print(command)
os.system(command)

