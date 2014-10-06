import node
import configparser
import sys

config = configparser.ConfigParser()

config.read("config.ini")

ip = config['SERVER1']['IP']
port = int(config['SERVER1']['PORT'])

node.node(ip, port)

# need to thread every node
#for server in config.sections():
#    ip = config[server]['IP']
#    port = int(config[server]['PORT'])
#    count = int(config[server]['NODES'])
#    for i in range(0, count):
#        try:
#            node.node(ip, port)
