import socket
import messages
import sys
import struct

class connection:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.lastMessageSend = 'none'
    def connect(self, ip, port):
        try:
            self.conn = self.sock.connect( (ip, port) )
        except OSError as e:
            if e.winerror == 10061:
                print('can not connect to server')
                return False
        return True
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
        except Exception as e:
            #10054 - host reset connection
            if e.errno == 10054:
                return { 'type': 'Disconnect', 'reason': 'server has broken connection'}
        except:
            print ("Unexpected error: {0}".format( sys.exc_info()[0] ) )
            return { 'type': 'Disconnect', 'reason': 'exception: {0}'.format( sys.exc_info()[0] ) }
        if not buff: # socket return null object
            return { 'type': 'Disconnect', 'reason': 'coz of error. Maybe server closed conn'}
        # messages with only type are not processing later
        typeOfMessage = buff
        result = { 'type' : messages.messageTypes[self.byteToInt(typeOfMessage)]}
        if  result['type'] == 'Reject':
            reason = self.readLenData()
            result.update( {'reason': reason} )
        if result['type'] == 'Task':
            parametrs = self.readLenData()
            result.update( {'parametrs': parametrs} )
            code = self.readLenData()
            result.update ({'code' : code})
        if result['type'] == 'Disconnect':
            reason = self.readLenData()
            reason = reason.decode("utf-8")
            result.update( {'reason': reason} )
        if ((result['type'] == 'Accept') and (self.lastMessageSend == 'Join')):
            name = self.readLenData()
            name = name.decode("utf-8")
            result.update( {'name' : name})
        print('CONNECTION: got message, Type = {} '.format(result['type']))
        return result

    def sendMessage(self, type, msg = ''):
        data = messages.createMessage(type, msg)
        if not data:
            print('CONNECTION: message was not sent, Type = {}'.format(type))
            return False
        self.sock.send(data)    
        print('CONNECTION: message sent, Type = {}'.format(type))
        self.lastMessageSend = type
        return True 
    def byteToInt(self, byte):
        result = struct.unpack('B', byte)[0]
        return result

