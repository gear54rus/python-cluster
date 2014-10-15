import subprocess
import threading

class worker:
    """description of class"""
    result = False
    proc = 'none'
    subprocthread = 'none'
    def __init__(self, pythonPath):
        self.pathToPython = pythonPath
        self.clear()
    def clear(self):
        self.proc = 'none'
        self.result = False
        self.subprocthread = 'none'
    def isFinished(self):
        if self.result:
            return True
        return False
    def stop(self):
        if not self.proc.poll():
            self.proc.terminate()
        return True
    def run(self, pathToAlgDir):
        self.subprocthread = threading.Thread(target=self.runSubprocWithCapturing, args=(pathToAlgDir,))
        self.subprocthread.start()
    def runSubprocWithCapturing(self, pathToAlgDir):
        self.proc = subprocess.Popen([self.pathToPython, 'alg.py', 'input'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT,
                             cwd=pathToAlgDir,)
        self.result, buff = self.proc.communicate()

