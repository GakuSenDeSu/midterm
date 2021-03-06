import numpy as np
import serial
import time

waitTime = 0.1

# song
song1 = [
    261, 261, 392, 392, 440, 440, 392,
    349, 349, 330, 330, 294, 294, 261,
    392, 392, 349, 349, 330, 330, 294,
    392, 392, 349, 349, 330, 330, 294,
    261, 261, 392, 392, 440, 440, 392,
    349, 349, 330, 330, 294, 294, 261]
note1 = [
    100, 100, 100, 100, 100, 100, 200,
    100, 100, 100, 100, 100, 100, 200,
    100, 100, 100, 100, 100, 100, 200,
    100, 100, 100, 100, 100, 100, 200,
    100, 100, 100, 100, 100, 100, 200,
    100, 100, 100, 100, 100, 100, 200
]
song2 = [
    261, 294, 330, 261, 261, 294, 330,
    261, 330, 349, 392, 330, 349, 392,
    392, 440, 392, 349, 330, 262, 392,
    440, 392, 349, 330, 262, 294, 392,
    261, 294, 392, 262]
note2 = [
    100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 200, 100, 100, 200,
    100, 100, 100, 100, 200, 200, 100,
    100, 100, 100, 200, 200, 200, 200,
    200, 200, 200, 200]
song3 = [
    392, 440, 392, 349, 330, 349, 392,
    294, 330, 349, 330, 349, 392,
    392, 440, 392, 349, 330, 349, 392,
    294, 392, 330, 262]
note3 = [
    200, 100, 100, 100, 100, 100, 300,
    100, 100, 300, 100, 100, 300,
    200, 100, 100, 100, 100, 100, 300,
    300, 300, 100, 400]

song = song1+note1+song2+note2+song3+note3
songlegth1 = len(song1)
songlegth2 = len(song2)
songlegth3 = len(song3)
songlength = 2 * (songlegth1+songlegth2+songlegth3)

songs = []
for num in song:
    songs.append(num)

# output formatter
formatter = lambda x: "%d" % x

# send the waveform table to K66F
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")
print("It may take about %d seconds ..." % (int(songlength * waitTime)))
for data in songs:
  s.write(bytes(formatter(data), 'UTF-8'))
  print("%d",bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)
s.close()
print("Signal sended")