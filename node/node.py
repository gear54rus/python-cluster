import connection
import worker
import sys
import os
import time
import shutil
import struct
import threading

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

def getModulesList():
        import subprocess
        proc = subprocess.Popen(["python", "-c", "help('modules')"], stdout=subprocess.PIPE)
        out = proc.communicate()[0]  #all modules divided by spaces
        out = out[76:len(out) - 158].decode("utf-8") #Cut trash
        out = out.replace('_', '')
        modulesArray = out.split()
        modulesList = ','.join(modulesArray) #modules divided by ,
        modulesList = modulesList
        return modulesList


class node:

    def __init__(self,ip,port, pythonPath):
       self.connection = connection.connection()
       if not self.connection.connect(ip, port):
           return
       cur_version = '{0}.{1}.{2};'.format(sys.version_info[0], sys.version_info[1], sys.version_info[2]) + getModulesList()
       self.connection.sendMessage('Join', cur_version)
       self.status = 'disconnected'
       self.codePath = ''
       self.result = ''
       self.name = 'defaultNodeName'
       self.worker = worker.worker(pythonPath)
       self.run()

    def getTimeUnix64(self):
        timestamp = time.time() # returns timestamp,[ms:3][micros:3]
        timestamp = int(timestamp * 1000)
        return timestamp

    def changeStatus(self, newStatus):
        #changing status if new status
        if newStatus != self.status:
            self.status = newStatus
            return True
        return False
    def getStatus(self):
        return self.status

    def stopWorkerTread(self):
        return True

    def run(self):
        var = 'true'
        while var:
            if (self.getStatus() == 'working') and self.worker.isFinished():
                self.connection.sendMessage('Finished', longintToBEByteStr(self.getTimeUnix64()) + ';'.encode() + self.worker.result)
                self.deleteTaskFolder()
                self.changeStatus('idle')
                self.worker.clear()
                print('NODE: Has finished task, waiting new task')
            try:
                msg = self.connection.checkSocket()
            except Exception:
                #dirty code
                continue
            if not msg:
                continue
            if msg['type'] != 'Task':
                #task maybe very huge and not readable
                print('NODE: got message: {0}'.format(msg))
            if msg['type'] == 'Accept':
                if 'name' in msg:
                    self.name = msg['name']
                    print('NODE: my name is {0}'.format(self.name))
                if self.getStatus() == 'disconnected':
                    self.changeStatus('idle')
            if msg['type'] == 'Reject':
                self.changeStatus('idle')
                print(msg['reason'])
            if msg['type'] == 'Status':
                self.connection.sendMessage('Status', statusenum[self.status])
            if msg['type'] == 'Task':
                print('NODE: got message: {0}'.format(msg['type']))
                if len(msg['code']) == 0:
                    print('NODE: got task, but receives task message with code length of 0. Waiting for new task.')
                    self.changeStatus('idle')
                    self.deleteTaskFolder()
                    self.connection.sendMessage('Accept')
                    continue
                if self.getStatus() == 'ready to start':
                    self.deleteTaskFolder()
                    self.changeStatus('idle')
                if self.getStatus() == 'idle':
                    self.parametrs = msg['parametrs']
                    self.code = msg['code']
                    self.codePath = self.createTask()
                    self.changeStatus('ready to start')
                    self.connection.sendMessage('Accept')
                else:
                    self.connection.sendMessage('Reject', 'Node is busy. Node status: ' + self.status)
            if msg['type'] == 'Start':
                if self.getStatus() == 'ready to start':
                    self.changeStatus('working')
                    self.connection.sendMessage('Start', longintToBEByteStr(self.getTimeUnix64()))
                    self.worker.run(self.codePath)
                else:
                    self.connection.sendMessage('Reject', 'Node is not ready to start. Node status: ' + self.status)
            if msg['type'] == 'Disconnect':
                self.status = 'disconnected'
                print('Server disconnected. Reason: ' + msg['reason'])
                if self.status == 'working':
                    self.stopWorkerTread()
                return 1
            if msg['type'] == 'Stop' and self.status == 'working':
                self.worker.stop()
                self.changeStatus('ready to start')
                self.connection.sendMessage('Accept')
        return 1

    def disconnect(self, reason):
        self.connection.sendMessage('Disconnect', reason)
        return 1



    def getTaskFullName(self):
        return self.name + '.task'

    def createTaskFolder(self):
        os.makedirs(self.getTaskFullName())
        return self.getTaskPath()

    def getTaskPath(self):
        curDir = os.getcwd()
        taskPath = curDir + '\\' + self.getTaskFullName()
        return taskPath

    def saveTaskFile(self, name, data):
        curDir = os.getcwd()
        os.chdir(self.getTaskPath())
        file = open(name, "wb")
        file.write(data)
        file.close()
        os.chdir(curDir)
        return True

    def createTask(self):
        taskPath = self.createTaskFolder()
        self.saveTaskFile('alg.py', self.code)
        self.saveTaskFile('input', self.parametrs)

        return taskPath

    def deleteTaskFolder(self):
        shutil.rmtree(self.getTaskPath(), ignore_errors = True, onerror = None)
        return True




