import subprocess
import threading

class worker:
    """description of class"""
    def __init__(self, pythonPath):
        self.pathToPython = pythonPath
        self.status = 'idle'
        self.clear()
    def clear(self):
        self.proc = 'none'
        self.result = False
        self.subprocthread = 'none'
        self._isFinished = False
    def isFinished(self):
        return self._isFinished
    def stop(self):
        if not self.proc == 'none':
            if not self.proc.poll():
                self.proc.terminate()
                self.clear()
                self.status = 'Terminated'
        return True
    def run(self, pathToAlgDir):
        self.status = 'working'
        self.subprocthread = threading.Thread(target=self.runSubprocWithCapturing, args=(pathToAlgDir,))
        self.subprocthread.start()
    def runSubprocWithCapturing(self, pathToAlgDir):
        self.proc = subprocess.Popen([self.pathToPython, 'alg.py', 'input'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT,
                             cwd=pathToAlgDir,)
        self.result, buff = self.proc.communicate()
        if self.status == 'Terminated':
            self.clear()
        else:
            self._isFinished = True
        self.status = 'idle'