import os
import sys
import io

class worker:
    """description of class"""
    def run(self, pathToAlgDir, name):
        curDir = os.getcwd()
        tempStdOut = sys.stdout
        myStdOut = io.StringIO()
        algName = 'alg.py'
        os.chdir(pathToAlgDir)
        sys.stdout = myStdOut
        exec(open(algName).read())
        os.chdir(curDir)
        sys.stdout = tempStdOut
        log = myStdOut.getvalue()
        return log

