import socket
import messages
import sys

class connection:
    def __init__(self, ip, port):
        self.sock = socket.socket()
        self.connect(ip, port)
    def connect(self, ip, port):
        self.conn, self.addr = self.sock.connect(ip, port)
        cur_version = sys.version_info[0:3]
        self.sendMessage('Join','{0}.{1}.{2}'.format(sys.version_info[0],sys.version_info[1],sys.version_info[2]))
    def readSocket(self, count):
        buff = self.sock.recv(count)
        return buff
    def readLenData(self):
        """ returns data only """
        length = messages.BEByteStrToInt(self.readSocket(4))
        data = sock.recv(length)
        return data
    def checkSocket(self):
        buff = self.sock.recv(1)
        if not buff: # empty socket
            return buff
        # messages with only type are not processing later
        typeOfMessage = buff
        result = { messages.getTypeOfMessage[typeOfMessage]}
        if result['type'] == 'Reject':
            reason = readLenData()
            result.update( {'reason': msgdata} )
        if result['type'] == 'Task':
            parametrs = readLenData()
            result.update( {'parametrs': parametrs} )
            code = readLenData()
            result.update ({'code' : code})
        if result['type'] == 'Disconnect':
            reason = readLenData()
            result.update( {'reason': reason} )
        print('CONNECTION: got message, Type = {} '.format(type))
        return result

    def sendMessage(type, msg):
        data = messages.createMessage(type,msg)
        if not data:
            print('CONNECTION: message was not sent, Type = {}'.format(type))
            return False
        self.sock.send(data)    
        print('CONNECTION: message sent, Type = {}'.format(type))
        return True 


