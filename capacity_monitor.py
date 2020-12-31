#!/usr/bin/env python
import serial
import time

ser=serial.Serial("/dev/ttyUSB0",9600,timeout=0.5)
#ser.parity='E'
#ser.baudrate = 9600
#print (ser.name)
#print (ser.port)
#print (ser.baudrate)
#print (ser.isOpen())
print (ser)
#ser.write(chr(0x00))
#n=ser.write(chr(0x31))
#n=ser.write(0x31)
#n=ser.write('l')
#n=ser.write(b'a')
#time.sleep(1)

while True:
    data=ser.read(1)
    if len(data)>0:
        print hex(ord(data))

#time.sleep(1)
#data = data.encode('utf-8')
#print (data)
if len(data)>0:
    print hex(ord(data))
#data = ser.readline()
#ser.write("hello")
ser.close()


exit
data=ser.read(1)
#time.sleep(1)
#data = data.encode('utf-8')
print (data)
#data = ser.readline()
#ser.write("hello")
#ser.close()
