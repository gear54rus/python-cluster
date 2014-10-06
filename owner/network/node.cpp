#include <QTimer>

#include "node.h"

Node::Node(QTcpSocket* socket) :
    QObject(nullptr)
{
    status = Connecting;
    this->socket = socket;
    message.reset();
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void Node::addTask(Task* task)
{
    switch(task->getType()) {
        case Task::GetStatus: {
            break;
        }
        case Task::Assign: {
            break;
        }
        case Task::Start: {
            break;
        }
        case Task::Stop: {
            break;
        }
    }
    tasks.enqueue(task);
}

void Node::kick()
{
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
    if(!available)
        return;
    try {
        switch(message.toParse) {
            case Message::MessagePart::Type: {
                quint8 t;
                QDataStream(buffer) >> t;
                if((t <= static_cast<quint8>(None)) || (t > static_cast<quint8>(Disconnect)))
                    throw "Unknown message type";
                message.type = static_cast<MessageType>(t);
                switch(message.type) {
                    case Accept: {
                        processMessage();
                        if(socket->bytesAvailable())
                            QTimer::singleShot(0, this, SLOT(readyRead));
                        break;
                    }
                    default: {
                        if(socket->bytesAvailable())
                            readyRead();
                    }
                }
                break;
            }
            case Message::MessagePart::Length: {
                QDataStream stream(buffer);
                switch(message.type) {
                    case Status: {
                        quint8 s;
                        stream >> s;
                        if((s <= static_cast<quint8>(Idle)) || (s > static_cast<quint8>(Working)))
                            throw "Unknown node status";
                        message.body.append(s);
                        processMessage();
                        if(socket->bytesAvailable())
                            QTimer::singleShot(0, this, SLOT(readyRead));
                        break;
                    }
                    default: {
                        quint8 forLength = 4 - buffer.length();
                        if(available >= forLength) {
                            quint32 l;
                            stream >> l;
                            if((l > MAX_MESSAGE_LENGTH) || (l == 0))
                                throw "Invalid message length";
                            message.length = l;
                            if(socket->bytesAvailable())
                                readyRead();
                        } else
                            buffer.append(socket->readAll());
                    }
                }
                break;
            }
            case Message::MessagePart::Body: {
                quint8 forBody = message.length - buffer.length();
                if(available >= forBody) {
                    message.body.append(socket->read(forBody));
                    processMessage();
                    if(socket->bytesAvailable())
                        QTimer::singleShot(0, this, SLOT(readyRead));
                } else
                    buffer.append(socket->readAll());
                break;
            }
        }
    } catch(QString reason) {
        emit malformedMessage(reason);
        socket->readAll();
        message.reset();
        buffer.clear();
        return;
    }
}

void Node::Message::reset()
{
    toParse = Type;
    type = None;
    length = 0;
    body.clear();
}

void Node::processMessage()
{
    switch(status) {
        case Connecting: {
            break;
        }
        case Idle: {
            break;
        }
        case ReadyToStart: {
            break;
        }
        case Working: {
            break;
        }
    }
    message.reset();
    buffer.clear();
}
