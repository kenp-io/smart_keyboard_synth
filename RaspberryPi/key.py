import socket
import sys
import time
import can
import os

os.system("sudo ip link set can0 down")
time.sleep(1)
os.system("sudo ip link set can0 up type can bitrate 500000")
time.sleep(1)

HOST, PORT = "REPLACE_WITH_COMPUTER_LOCAL_IP", 9999

bitrate = 500000
can.rc['interface'] = 'socketcan'
can.rc['channel'] = 'can0'
can.rc['bitrate'] = bitrate
bus = can.interface.Bus()

# SOCK_DGRAM is the socket type to use for UDP sockets
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# As you can see, there is no connect() call; UDP has no connections.
# Instead, data is directly sent to the recipient via sendto().

while 1==1:
    msg = bus.recv(2)
    if msg == None:
        print("None")
    elif len(msg) is not 3:
        print("error")
    else:
        #print(f"Message received: {msg}")
         data = list(msg.data)
         #print(f"Message received: {data}")
         msg = [str(data[0]), str(data[1]), str(data[2])]
         msg = ','.join(msg)
         sock.sendto(bytes(msg, "utf-8"), (HOST, PORT))
         print(f"Message sent: {msg}")
    time.sleep(0.001)

#received = str(sock.recv(1024), "utf-8")

#print("Sent:     {}".format(data))
#print("Received: {}".format(received))
