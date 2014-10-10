import struct 

messageTypes = {
'Accept' : 0x1,
'Reject' : 0x2,
'Join' : 0x3,
'Status' : 0x4,
'Task' : 0x5,
'Start' : 0x6,
'Stop' : 0x7,
'Finished' : 0x8,
'Disconnect' : 0x9,

#reverse

0x1 : 'Accept',
0x2 : 'Reject',
0x3 : 'Join',
0x4 : 'Status',
0x5 : 'Task',
0x6 : 'Start',
0x7 : 'Stop',
0x8 : 'Finished',
0x9 : 'Disconnect'
}

def intToBEByteStr(number):
    return struct.pack('>I', number)

def longintToBEByteStr(number):
    return struct.pack('>Q', number)

def BEByteStrToInt(str):
    return struct.unpack('>I', str)[0]

def createMessage(messTypeName, messData = ''):
    result = False
    if messTypeName == 'Join':
        result = bytes([messageTypes[messTypeName]]) + intToBEByteStr(len(messData)) + messData.encode('utf-8')
    if messTypeName == 'Accept':
        #accept message data is 64bit unix time
        result = bytes([messageTypes[messTypeName]]) + longintToBEByteStr(messData)
    if messTypeName == 'Reject':
        result = bytes([messageTypes[messTypeName]]) + intToBEByteStr(len(messData)) + messData.encode('utf-8')
    if messTypeName == 'Status':
        result = bytes([messageTypes[messTypeName]] )+ bytes( [messData] )
    if messTypeName == 'Finished':
        result = bytes([messageTypes[messTypeName]]) + intToBEByteStr(len(messData)) + messData
    if messTypeName == 'Disconnect':
        result = bytes([messageTypes[messTypeName]]) + intToBEByteStr(len(messData)) + messData.encode('utf-8')
    if messTypeName == 'Start':
        result = bytes([messageTypes[messTypeName]]) + messData
    return result

def getTypeOfMessage(msg):
    return int(msg[0])

def GetMsgLenData(msg):
    """ returns len data from message and checks it. Input = len|data|anybytes"""
    length = BEByteStrToInt(msg[0:4])
    msgdata = msg[4 : 4 + length]
    if len(msgdata) != length:
        print('ERROR: len(msgdata) != length in message')
    return length, msgdata


def readMessage(msg):
    """ returns {} with key 'type', other keys are defined by messageType"""
    result = {}
    result.update( {'type': getTypeOfMessage(msg) }) 
    msg = msg[1:]
    if result['type'] == 'Reject':
        length, msgdata = GetMsgLenData(msg)
        result.update( {'length': length, 'reason': msgdata} )
    if result['type'] == 'Task':
        parlength, pardata = GetMsgLenData(msg)
        msg = msg[parlength:]
        codelength, codedata = GetMsgLenData(msg)
        result.update ({'parametrs' : pardata, 'code' : codedata })
    return result