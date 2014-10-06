import socket
import messages
import sys
import struct

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
        data = self.sock.recv(length)
        return data
    def checkSocket(self):
        try:
            buff = self.sock.recv(1)
        except:
            #if we can get data from socket then tell to node disconnect
            print ("Unexpected error: {0}".format( sys.exc_info()[0] ) )
            return { 'type': 'Disconnect', 'reason': 'coz of error'}
        if not buff: # empty socket
            return buff
        # messages with only type are not processing later
        typeOfMessage = buff
        result = { 'type' : messages.messageTypes[self.byteToInt(buff)]}
        if  result['type'] == 'Reject':
            reason = self.readLenData()
            result.update( {'reason': msgdata} )
        if result['type'] == 'Task':
            parametrs = self.readLenData()
            result.update( {'parametrs': parametrs} )
            code = self.readLenData()
            result.update ({'code' : code})
        if result['type'] == 'Disconnect':
            reason = self.readLenData()
            result.update( {'reason': reason} )
        print('CONNECTION: got message, Type = {} '.format(result['type']))
        return result

    def sendMessage(self, type, msg = ''):
        data = messages.createMessage(type, msg)
        if not data:
            print('CONNECTION: message was not sent, Type = {}'.format(type))
            return False
        self.sock.send(data)    
        print('CONNECTION: message sent, Type = {}'.format(type))
        return True 
    def byteToInt(self, byte):
        result = struct.unpack('B', byte)[0]
        return result

