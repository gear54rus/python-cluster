import socket
import messages

sock = socket.socket()
sock.bind(('', 9090))
print("Waiting for connect.Please, run client")
sock.listen(1)
conn, addr = sock.accept()

print('Connected to ' + addr.__str__())

while True:
    command = input('Enter command:')
    helpC = "Commands list:\n Accept \nReject \nJoin \nLeave\nStatus\nTask\nStart\nStop\nFinished\nDisconnect"
    if command == 'Disconnect':
        conn.close()
        print('Connection closed')
        break
    if command == 'Help' or command == '?':
        print(helpC)
        continue
    data = conn.recv(1024)
    if not data:
        continue
    data = data.decode('UTF-8')
    data = data.upper()
    data = bytes(data,'UTF-8')
    conn.send(data)


#hardcoded messages
def sendMessage(type):
    if type == 'Accept':
        conn.send(bytes[messages.messageTypes['Accept']])

    print('message of type {0} sent'.format(type))