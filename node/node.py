import connection
import worker
import messages
import sys
import datetime
import os
import random
import time

statusenum = {
0x1: 'idle',
0x2: 'ready to start',
0x3: 'working',
0x4: 'disconnected',

'idle': 0x1,
'ready to start': 0x2,
'working': 0x3,
'disconnected': 0x4
}


def longintToBEByteStr(number):
    return struct.pack('>Q', number)



class node:

    def __init__(self,ip,port):
       self.connection = connection.connection(ip, port)
       cur_version = '{0}.{1}.{2};'.format(sys.version_info[0],sys.version_info[1],sys.version_info[2])
       self.connection.sendMessage('Join', cur_version)
       self.status = 'disconnected'
       self.codePath = ''
       self.result = ''
       self.name = 'defaultNodeName'
       self.worker = worker.worker()
       self.run()
    def getTimeUnix64(self):
        timestamp = datetime.datetime.now()
        timestamp = time.mktime(timestamp.timetuple())*1e3 + timestamp.microsecond/1e3
        timestamp = int (timest * 1000)
        return timestamp
    def run(self):
        var = 'true'
        while var:
            msg = self.connection.checkSocket()
            if not msg:
                continue
            if msg['type'] != 'Task':
                #imaged that 256MB printed in stdout 
                print('NODE: got message: {0}'.format(msg))
            if msg['type'] == 'Accept':

                try:
                    self.name = msg['name']
                    break
                except:
                    salt = random.randint(1,9999)
                    self.name = 'commonNode' + salt.__str__()
                    break

                self.status = 'idle'
            if msg['type'] == 'Reject':
                self.status = 'idle'
                print(msg['reason'])
            if msg['type'] == 'Status':
                self.connection.sendMessage('Status', statusenum[ self.status ] )
            if msg['type'] == 'Task':
                print('NODE: got message: {0}'.format(msg['type']))
                if self.status == 'idle':
                    self.parametrs = msg['parametrs']
                    self.code = msg['code']
                    self.codePath = self.createTask()
                    self.status = 'ready to start'
                    self.connection.sendMessage('Accept')
                else:
                    self.connection.sendMessage('Reject','Node is busy. Node status: ' + self.status)
            if msg['type'] == 'Start':
                if self.status == 'ready to start':
                    self.status = 'working'
                    self.connection.sendMessage('Accept', self.getTimeUnix64())
                    self.result = self.worker.run(self.codePath)
                    self.connection.sendMessage('Finished', longintToBEByteStr(self.getTimeUnix64()) + ','.encode() + self.result)
                else:
                    self.connection.sendMessage('Reject','Node is not ready to start. Node status: ' + self.status)
            if msg['type'] == 'Disconnect':
                self.status = 'disconnected'
                print('Server disconnected. Reason: ' + msg['reason'])
                return 1

        return 1

    def disconnect(self,reason):
        self.connection.sendMessage('Disconnect',reason)
        return 1

    def createTask(self):
        code = self.code
        parametrs = self.parametrs
        algName = 'alg.py'
        curDir = os.getcwd();
        taskFullName = self.name + '.task'
        os.makedirs(taskFullName)
        taskPath = curDir + '\\' + taskFullName
        os.chdir(taskPath)
        file = open(algName, "wb")
        file.write(code)
        file.close()
        file = open("input", "wb")
        file.write(parametrs)
        file.close()
        os.chdir(curDir)
        return taskPath


