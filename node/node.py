import connection
import worker
import messages
import sys
import datetime
import os
import random
import time
import shutil
import struct

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
       self.connection = connection.connection()
       if not self.connection.connect(ip, port):
           return
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
        timestamp = int (timestamp * 1000)
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
                if 'name' in msg:
                    self.name = msg['name']
                    print('NODE: my name is {0}'.format(self.name))
                self.status = 'idle'
            if msg['type'] == 'Reject':
                self.status = 'idle'
                print(msg['reason'])
            if msg['type'] == 'Status':
                self.connection.sendMessage('Status', statusenum[ self.status ] )
            if msg['type'] == 'Task':
                print('NODE: got message: {0}'.format(msg['type']))
                if (len(msg['code']) == 0):
                    print('NODE: got task, but receives task message with code length of 0. Waiting for new task.')
                    self.status = 'idle'
                    break 
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
                    self.connection.sendMessage('Start', self.getTimeUnix64())
                    self.result = self.worker.run(self.codePath)
                    self.connection.sendMessage('Finished', longintToBEByteStr(self.getTimeUnix64()) + ';'.encode() + self.result)
                    self.deleteTaskFolder()
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



    def getTaskFullName(self):
        return self.name + '.task'

    def createTaskFolder(self):
        os.makedirs(self.getTaskFullName())
        return self.getTaskPath()

    def getTaskPath(self):
        curDir = os.getcwd();
        taskPath = curDir + '\\' + self.getTaskFullName()
        return taskPath

    def saveTaskFile(self, name, data):
        curDir = os.getcwd();
        os.chdir(self.getTaskPath())
        file = open(name, "wb")
        file.write(data)
        file.close()
        os.chdir(curDir)
        return True

    def createTask(self):
        code = self.code
        parametrs = self.parametrs
        algName = 'alg.py'
        taskPath = self.createTaskFolder()
        self.saveTaskFile('alg.py', self.code)
        self.saveTaskFile('input', self.parametrs)

        return taskPath
    def deleteTaskFolder(self):
        shutil.rmtree(self.getTaskPath(), ignore_errors = True, onerror = None)
        return True

