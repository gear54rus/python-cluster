import threading
import os

class worker:
    """description of class"""
    def run(self, pathToAlgDir):
        curDir = os.getcwd()
        algName = 'alg.py'
        os.chdir(pathToAlgDir)
        exec(open(algName).read())
        with open('log', 'rb') as content_file:
            log = content_file.read()
        os.chdir(curDir)
        return log

