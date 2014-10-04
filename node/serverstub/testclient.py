import socket
import messages

sock = socket.socket()
sock.connect(('localhost', 9090))
while True:
    command = input('Enter command:')
    if command == 'Receive':
        data = sock.recv(1024)
        if data.decode('UTF-8') == 'Disconnect':
            print('Server closed the connection')
            break
        if data:
            print(data)
        else:
            print('Server sent empty message')
    if command == 'Send':
        sock.send(bytes('Disconnect', 'UTF-8'))
    continue



sock.close()

print('End of the programm')