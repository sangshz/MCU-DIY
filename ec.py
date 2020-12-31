#!/usr/bin/env python
import serial
import time
import sys
import binascii

ser=serial.Serial("/dev/ttyUSB0",9600*2,timeout=0.01)
#ser.parity='E'
#ser.baudrate = 9600
#print (ser.name)
#print (ser.port)
#print (ser.baudrate)
#print (ser.isOpen())
#print (ser)
#ser.write(chr(sys.argv[1]))
ser.write(chr(0xa5))
#n=ser.write(chr(0x31))
#n=ser.write(0x31)
#n=ser.write('l')
#n=ser.write(b'a')
#time.sleep(1)


while True:
    data=ser.read(6)
    data0=data[0:2]
    data1=data[2:6]
    #data2=data[4:6]
    #data1=ser.read(2)
    #data2=ser.read(2)
    if len(data)>0:
        #print binascii.b2a_hex(data)
        #print int(binascii.b2a_hex(data0),16), int(binascii.b2a_hex(data),16)/1000.0
        us=int(binascii.b2a_hex(data0),16)
        res1=int(binascii.b2a_hex(data1),16)
        #res2=int(binascii.b2a_hex(data2),16)
        if res1>50 and res1<1000000 :
            print us,res1/100.0
            sys.stdout.flush()
        #if res1!=0:
        #    print us,res1,res2,res2*1.0/res1
        #else:
        #    print us,res1,res2,999999

ser.close()

exit()

while True:
    data=ser.read(4)
    if len(data)>0:
        res=int(binascii.b2a_hex(data),16)
        print res/10.0

ser.close()
exit()



