import socket
import messages
import sys

class connection:
    def __init__(self, ip, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( ip, port )
    def connect(self, ip, port):
        self.conn = self.sock.connect( ('127.0.0.1', 9090) )

        #cur_version = '{0}.{1}.{2}'.format(sys.version_info[0],sys.version_info[1],sys.version_info[2])

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
        result = { messages.getTypeOfMessage(typeOfMessage)}
        if  'Reject' not in result:
            reason = readLenData()
            result.update( {'reason': msgdata} )
        if 'Task' not in result:
            parametrs = readLenData()
            result.update( {'parametrs': parametrs} )
            code = readLenData()
            result.update ({'code' : code})
        if 'Disconnect' not in result:
            reason = readLenData()
            result.update( {'reason': reason} )
        print('CONNECTION: got message, Type = {} '.format(type))
        return result

    def sendMessage(self, type, msg):
        data = messages.createMessage(type,msg)
        if not data:
            print('CONNECTION: message was not sent, Type = {}'.format(type))
            return False
        self.sock.send(data)    
        print('CONNECTION: message sent, Type = {}'.format(type))
        return True 


