import socket
import messages
#hardcoded messages
def sendMessage(type, data):
    if type == 'Accept':
        conn.send(bytes[messages.createMessage['Accept']])
    if type == 'Reject':
        conn.send(bytes[messages.createMessage['Reject', data]])
    if type == 'Status':
        conn.send(bytes[messages.createMessage['Status']])
    if type == 'Start':
        conn.send(bytes[messages.createMessage['Start']])
    print('message of type {0} sent'.format(type))

sock = socket.socket()
sock.bind(('', 9090))
print("Waiting for connect.Please, run client")
sock.listen(1)
conn, addr = sock.accept()

print('Connected to ' + addr.__str__())

while True:
    command = input('Enter command:')
    helpC = "Commands list:\nReceive - trying to receive message from client\n Accept - send accept command \nReject - send reject command \nJoin \nLeave\nStatus\nTask\nStart\nStop\nFinished\nDisconnect\nQuit"
    if command == 'Disconnect':
        conn.send(bytes('Disconnect', 'UTF-8'))
        print('Connection closed.')
        print("Waiting for connect.")
        conn.close()
        sock.listen(1)
        conn, addr = sock.accept()
        print('Connected to ' + addr.__str__())
        continue
    if command == 'Help' or command == '?':
        print(helpC)
        continue
    if command == 'Quit':
        conn.close()
        print('Connection closed. Exiting.')
        break
    if command == 'Receive':
        data = conn.recv(1024)
        if not data:
            print('Can not receive data from client, try again.')
            continue
        else:
            print('Data received from client. Continue...')
            continue
    if command == 'Accept':
        sendMessage('Accept', '')
        continue
    if command == 'Reject':
        reason = 'Unknown reason'
        sendMessage('Reject',reason)
        continue
    if command == 'Status':
        sendMessage('Status')
        continue
    if command == 'Task':
        with open('algTestRead.py', 'r') as content_file:
            code = content_file.read()
        parameters = '14'
        task = bytes([messages.messageTypes['Task']]) + messages.intToBEByteStr(len(parameters)) + parameters.encode('utf-8') + messages.intToBEByteStr(len(code)) + code.encode('utf-8')
        conn.send(task)
        print('Task sent')
        continue
    if command == 'Start':
        sendMessage('Start')
        continue
    else:
        conn.send(bytes(command, 'UTF-8'))




