import unittest
import worker


pathTo = 'C:\\Py\\VMBuffer\\task1\\'
algFile = 'alg1.py'
outputFile = 'log'

class Test_test1(unittest.TestCase):
    def test_A(self):
        worker_obj = worker.worker()
        worker_obj.run(pathTo + algFile)
        f = open(pathTo + outputFile)
        lastline = 0
        for line in f:
            lastline = line
            if (lastline == 'done'):
                print(str)
                self.fail()

if __name__ == '__main__':
    unittest.main()
