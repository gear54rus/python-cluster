import socket
import messages

sock = socket.socket()
sock.connect(('localhost', 9090))

while True:
    data = sock.recv(1024)
    if data.decode('UTF-8') == 'Disconnect':
        print('Server closed the connection')
        break
    if data:
        print(data)


sock.close()

print('End of the programm')