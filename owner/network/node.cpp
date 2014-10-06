#include <QTimer>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    switch(task->getType()) {
        case Task::GetStatus: {
            stream << static_cast<quint8>(Status);
            break;
        }
        case Task::Assign: {
            auto t = static_cast<AssignTask*>(task);
            //quint32 length = sizeof(quint32) * 2 + inputLength + codeLenght; // by stupid request
            stream << static_cast<quint8>(Job) <</* static_cast<quint32>(length) << static_cast<quint32>(t->input.length()) <<*/ t->input /*<< static_cast<quint32>(t->code.length())*/ << t->code;
            break;
        }
        case Task::Start: {
            stream << static_cast<quint8>(Start);
            break;
        }
        case Task::Stop: {
            stream << static_cast<quint8>(Stop);
            break;
        }
    }
    socket->write(message);
    socket->flush();
    tasks.enqueue(task);
}

void Node::kick()
{
    QString reason("Kicked by owner");
    QByteArray message;
    message.append(static_cast<quint8>(Disconnect));
    message.append(static_cast<quint32>(reason.length()));
    message.append(reason);
    socket->write(message);
    socket->flush();
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
                QDataStream(socket->read(sizeof(quint8))) >> t;
                if((t < static_cast<quint8>(Accept)) || (t > static_cast<quint8>(Disconnect)))
                    throw "Unknown message type";
                message.type = static_cast<MessageType>(t);
                message.toParse = Message::MessagePart::Length;
                switch(message.type) {
                    case Accept: {
                        if(!processMessage())
                            return;
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
                switch(message.type) {
                    case Status: {
                        buffer.append(socket->read(1));
                        QDataStream stream(buffer);
                        quint8 s;
                        stream >> s;
                        if((s < static_cast<quint8>(Idle)) || (s > static_cast<quint8>(Working)))
                            throw "Unknown node status";
                        message.body.append(s);
                        if(!processMessage())
                            return;
                        if(socket->bytesAvailable())
                            QTimer::singleShot(0, this, SLOT(readyRead));
                        break;
                    }
                    default: {
                        quint8 forLength = sizeof(quint32) - buffer.length();
                        if(available >= forLength) {
                            buffer.append(socket->read(forLength));
                            QDataStream stream(buffer);
                            quint32 l;
                            stream >> l;
                            if((l > MAX_MESSAGE_LENGTH) || (l == 0))
                                throw "Invalid message length";
                            message.length = l;
                            buffer.clear();
                            message.toParse = Message::MessagePart::Body;
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
                    if(!processMessage())
                        return;
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

int Node::taskIndex(Task* task)
{
    for(qint32 i; i < tasks.length(); i++) {
        if(tasks[i]->getType() == task->getType())
            return i;
    }
    return -1;
}

bool Node::processMessage()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    bool processFurther = true;
    switch(status) {
        case Connecting: {
            QRegularExpressionMatch match = QRegularExpression("^(\\d+\\.\\d+\\.\\d+);((?:[a-z_][a-z0-9_]*,?)*)$").match(this->message.body);
            if(match.hasMatch()) {
                version = match.captured(1);
                modules = match.captured(2).split(',', QString::SkipEmptyParts);
                status = Idle;
                stream << static_cast<quint8>(Accept);
                emit joined();
            } else {
                QString reason("Invalid join message");
                stream << static_cast<quint8>(Reject) << reason.toLatin1();
                emit joinError(reason);
            }
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
    if(!processFurther)
        QObject::disconnect();
    socket->write(message);
    socket->flush();
    this->message.reset();
    buffer.clear();
    return processFurther;
}
