import numpy as np
import serial
import time

waitTime = 0.1

# Little star
# haven't have good idea to print three songs on screen, and catched by Mbed
song = np.array([
261, 261, 392, 392, 440, 440, 392, 
349, 349, 330, 330, 294, 294, 261, 
392, 392, 349, 349, 330, 330, 294, 
392, 392, 349, 349, 330, 330, 294, 
261, 261, 392, 392, 440, 440, 392, 
349, 349, 330, 330, 294, 294, 261])
noteLength = np.array([
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2])
Quiker = 0.5*np.array([
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2])
Slower = 2*np.array([
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2, 
1, 1, 1, 1, 1, 1, 2])

signalTable = [song, noteLength, song, Quicker, song, Slower]
formatter = lambda x: "%.3f" % x
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")
for data in signalTable:
  s.write(bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)
s.close()
print("Signal sended")