import socket
import select
import pyaudio
import wave
import _thread
import sys
import gi

class ChatServer:
    def __init__(self):
		
        so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        so.connect(("8.8.8.8", 80))
        adrs = so.getsockname()[0]
        so.close()
		
		
        self.CONNECTION_LIST = []
        self.chat_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.chat_server_socket.bind((adrs, 50000))
        self.chat_server_socket.listen(5)

        self.CONNECTION_LIST.append(self.chat_server_socket)

        print("Server Started!")

    def broadcast(self, sock, data):
        for current_socket in self.CONNECTION_LIST:
            if current_socket != self.chat_server_socket and current_socket != sock:
                try:
                    current_socket.send(data)
                except:
                    pass

    def run(self):
        while True:
            rlist, wlist, xlist = select.select(self.CONNECTION_LIST, [], [])

            for current_socket in rlist:
                if current_socket is self.chat_server_socket:
                    (new_socket, address) = self.chat_server_socket.accept()
                    self.CONNECTION_LIST.append(new_socket)
                    print("connected to the server" + str(address))
                else:
                    try:
                        data = current_socket.recv(1024)
                        self.broadcast(current_socket, data)
                    except socket.error:
                        print("left the server" + str(address))
                        current_socket.close()
                        self.CONNECTION_LIST.remove(current_socket)


if __name__ == "__main__":
    ChatServer().run()
