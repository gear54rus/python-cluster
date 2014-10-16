import sys

class fibonachiNumbers:
	def __init__(self, number):
		self.a = 0
		self.b = 1
		for i in range (0, number):
			self.procNext()
	def procNext(self):	
		c = self.a + self.b
		self.a = self.b
		self.b = c
	def get(self):
		return self.b


import time

start_time = time.time()

number = 100000
fnumber = fibonachiNumbers(number).get()

elapsed_time = time.time() - start_time

print('{0} number of fibonachi is {1}'.format(number, fnumber))
print('elapsed time {0}'.format(elapsed_time))
