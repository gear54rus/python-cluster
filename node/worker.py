import os
import sys
import io

class worker:
    """description of class"""
    result = False
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
        self.result = myStdOut.getvalue()
