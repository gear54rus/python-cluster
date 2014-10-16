import node
import configparser
import sys

config = configparser.ConfigParser()

config.read("config.ini")

ip = config['SERVER']['IP']
port = int(config['SERVER']['PORT'])
pythonPath = sys.executable

node.node(ip, port, pythonPath)