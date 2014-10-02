import connection
import worker
import messages

typeJoin = messages.createMessage('Join', '3.4.1')
class node:

    self.status = 'idle'
    def connect(ip, port):
        self.conn = connection(ip, port)


    def run():
        while var == 1:
            msg = conn.checkSocket()
            if not msg:
                continue
            if msg['type'] == 'Status':
                # Send status
                conn.send('Status',self.status)
        return 1