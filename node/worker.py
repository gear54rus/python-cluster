import threading
import os

class worker:
    """description of class"""
    def run(self, pathToAlgDir):
        curDir = os.getcwd()
        algName = 'alg.py'
        os.chdir(pathToAlgDir)
        exec(open(algName).read())
        os.chdir(curDir)
        return True

