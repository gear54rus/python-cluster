import connection
import worker
import messages
import sys
import datetime
import os
import random

class node:

    def __init__(self,ip,port):
       self.connect = connection.connect(ip, port)
       self.connect.send('Join', sys.version[0:5].__str__())
       self.status = 'disconnected'
       self.codePath = ''
       self.result = ''


    def run(self):
        var = 'true'
        while var:
            msg = self.connect.checkSocket()
            if not msg:
                continue
            if msg['type'] == 'Accept':
                self.status = 'idle'
            if msg['type'] == 'Reject':
                self.status = 'idle'
                print(msg['reason'])
            if msg['type'] == 'Status':
                self.connect.send('Status', self.status)
            if msg['type'] == 'Task':
                if self.status == 'idle':
                    self.parametrs = msg['parametrs']
                    self.code = msg['code']
                    self.codePath = self.createTask(self)
                    self.status = 'ready to start'
                    self.connect.send('Accept')
                else:
                    self.connect.send('Reject','Node is busy. Node status: ' + self.status)
            if msg['type'] == 'Start':
                if self.status == 'ready to start':
                    self.status = 'working'
                    self.connect.send('Accept',datetime.datetime.now())
                    self.result = worker.run(self.codePath)
                    self.connect.send('Finished',datetime.datetime.now() + ',' + self.result)
                else:
                    self.connect.send('Reject','Node dont ready to start. Node status: ' + self.status)
            if msg['type'] == 'Disconnect':
                self.status = 'disconnected'
                print('Server disconnected. Reason: ' + msg['reason'])
        return 1

    def disconnect(self,reason):
        self.connect.send('Disconnect',reason)
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
        file = open(algName, "w")
        file.write(code)
        file.close()
        file = open("import", "w")
        file.write(parametrs)
        file.close()
        os.chdir(curDir)
        return taskPath