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
        messages.createMessage('Join','{0}.{1}.{2}'.format(sys.version_info[0],sys.version_info[1],sys.version_info[2])) 
    def readSocket(self, count):
        buff = self.sock.recv(count)
        return buff
    def checkSocket(self):
        buff = self.sock.recv(1)
        if not buff: # empty socket
            return buff
        # messages with only type are not processing here
        typeOfMessage = buff
        result = { messages.getTypeOfMessage[typeOfMessage]}
        if result['type'] == 'Reject':
            length, msgdata = GetMsgLenData(msg)
            result.update( {'length': length, 'reason': msgdata} )
        if result['type'] == 'Task':
            parlength, pardata = GetMsgLenData(msg)
            msg = msg[parlength:]
            codelength, codedata = GetMsgLenData(msg)
            result.update ({'parametrs' : pardata, 'code' : codedata })
        return result
    def sendMessage(type, msg):
        #TODO: need to create message 
        self.sock.send(msg)     


