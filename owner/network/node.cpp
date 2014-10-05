#include <QtEndian>

#include "node.h"

Node::Node(QTcpSocket* socket) :
    QObject(nullptr)
{
    status = Connecting;
    this->socket = socket;
    message.reset();
    QObject::connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void Node::addTask(Task* task)
{
    switch(task->getType()) {
        case Task::Assign: {
        }
    }
    tasks.enqueue(task);
}

Node::~Node()
{
    socket->deleteLater();
    foreach(Task * v, tasks) {
        delete v;
    }
}

void Node::readyRead()
{
    quint64 available = socket->bytesAvailable();
    if(message.type == None) {
        quint8 forHeader = 5 - buffer.length();
        if(available >= forHeader) {
            buffer.append(socket->read(forHeader));
            QDataStream stream(buffer);
            quint8 t;
            quint32 l;
            stream >> t;
            stream >> l;
            try {
                if((t <= static_cast<quint8>(None)) || (t > static_cast<quint8>(Disconnect)))
                    throw "Unknown message type";
                if((l > MAX_MESSAGE_LENGTH) || (l == 0))
                    throw "Invalid message length";
            } catch(QString reason) {
                emit malformedMessage(reason);
                socket->readAll();
                message.reset();
                return;
            }
            message.type = static_cast<MessageType>(t);
            message.length = l;
            buffer.clear();
            if(available > forHeader)
                readyRead();
        } else
            buffer.append(socket->readAll());
    } else {
        quint8 forMessage = message.length - buffer.length();
        if(available >= message.length) {
            buffer.append(socket->read(forMessage));
            message.body = buffer;
            buffer.clear();
            processMessage();
        } else
            buffer.append(socket->readAll());
    }
}

void Node::Message::reset()
{
    type = None;
    length = 0;
    body.clear();
}

void Node::processMessage()
{
    message.reset();
}
