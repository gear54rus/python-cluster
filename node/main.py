import node
import configparser
config = configparser.ConfigParser()

config.read("config.ini")

for server in config.sections():
    ip = config[server]['IP']
    port = int(config[server]['PORT'])
    count = int(config[server]['NODES'])
    for i in range(0, count):
        node.node(ip, port)
