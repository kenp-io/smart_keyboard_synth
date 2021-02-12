import socketserver
import rtmidi

class MyUDPHandler(socketserver.BaseRequestHandler):
    """
    This class works similar to the TCP handler class, except that
    self.request consists of a pair of data and client socket, and since
    there is no connection the client address must be given explicitly
    when sending data back via sendto().
    """

    def handle(self):

        global midiOut

        data = self.request[0].strip().decode("utf-8")
        socket = self.request[1]
        data = data.split(',')
        data = [int(i) for i in data]
        print(data)
        midiOut.send_message(data)
        #socket.sendto(data.upper(), self.client_address)

if __name__ == "__main__":

    HOST, PORT = "REPLACE_WITH_COMPUTER_LOCAL_IP", 9999

    midiOut = rtmidi.MidiOut()
    available_ports = midiOut.get_ports()

    if available_ports:
        for i in available_ports:
            print(i)
        midiOut.open_port(0)
    else:
        print('NO MIDI INPUT PORTS!')
        exit()

    with socketserver.UDPServer((HOST, PORT), MyUDPHandler) as server:
        server.serve_forever()
