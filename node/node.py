import connection
import worker
import messages
import sys
import datetime
import os
import random

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


class node:

    def __init__(self,ip,port):
       self.connection = connection.connection(ip, port)
       cur_version = '{0}.{1}.{2};'.format(sys.version_info[0],sys.version_info[1],sys.version_info[2])
       self.connection.sendMessage('Join', cur_version)
       self.status = 'disconnected'
       self.codePath = ''
       self.result = ''
       self.worker = worker.worker()
       self.run()


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
                self.status = 'idle'
            if msg['type'] == 'Reject':
                self.status = 'idle'
                print(msg['reason'])
            if msg['type'] == 'Status':
                self.connection.sendMessage('Status', statusenum[self.status] )
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
                    self.connection.sendMessage('Accept',datetime.datetime.now())
                    self.result = self.worker.run(self.codePath)
                    self.connection.sendMessage('Finished' ,datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S").encode() + ','.encode() + self.result)
                else:
                    self.connection.sendMessage('Reject','Node dont ready to start. Node status: ' + status)
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
        NumberOfAttempts = 0
        maxNumberOfAttempts = 10
        while True:
            try:
                salt = random.randint(1,9999)
                taskFullName = 'task' + salt.__str__()
                os.makedirs(taskFullName)
                break
            except OSError:
                if NumberOfAttempts < maxNumberOfAttempts:
                    print("Oops!  That was no valid number.  Try again...")
                    NumberOfAttempts += 1
                else:
                    print("Too many attempts. Break")
                    break
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


