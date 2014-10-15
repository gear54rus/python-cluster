import os
import sys
import io
import psutil
import subprocess
from subprocess import Popen, PIPE
import threading

class worker:
    """description of class"""
    def __init__(self, pythonPath):
        self.pathToPython = pythonPath
    result = False
    pid = 'none'
    def isFinished(self):
        process = psutil.Process(self.pid)
        if process.is_running():
            return False
        return True
    def stop(self):
        process = psutil.Process(self.pid)
        self.pid = 'none'
        for proc in process.get_children(recursive=True):
            proc.kill()
        process.kill()
        return True
    def run(self, pathToAlgDir):
        self.subprocthread = threading.Thread(target=self.runSubprocWithCapturing, args=(pathToAlgDir,))
        self.subprocthread.start()
    def runSubprocWithCapturing(self, pathToAlgDir):
        p = subprocess.Popen([self.pathToPython, 'alg.py'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT,
                             cwd=pathToAlgDir,)
        self.pid = p.pid
        self.result, buff = p.communicate()