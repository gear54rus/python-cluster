import node
import configparser
import sys

config = configparser.ConfigParser()

config.read("config.ini")

ip = config['SERVER']['IP']
port = int(config['SERVER']['PORT'])
pythonPath = config['PYTHON']['PATH']

node.node(ip, port, pythonPath)