#include <QTimer>
#include <QChar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDateTime>

#include "node.h"

QStringList Node::typeText(QStringList("None") << "Accept" << "Reject" << "Join" << "Status" << "Job" << "Start" << "Stop" << "Finish" << "Disconnect");
QStringList Node::statusText(QStringList("Connecting") << "Idle" << "Ready to start" << "Working");

Node::Node(QTcpSocket* socket, quint32 id, QString name) :
    QObject(nullptr)
{
    status = Connecting;
    name[0] = name[0].toUpper();
    this->id = id;
    this->name = name.toLatin1();
    this->socket = socket;
    message.reset();
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
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
            stream << static_cast<quint8>(Job);
            if(t->inputBytes.length())
                stream << t->inputBytes;
            else
                stream << quint32(0);
            if(t->codeBytes.length())
                stream << t->codeBytes;
            else
                stream << quint32(0);
            break;
        }
        case Task::Start: {
            jobStartedAtLocal = QDateTime::currentMSecsSinceEpoch();
            stream << static_cast<quint8>(Start);
            break;
        }
        case Task::Stop: {
            stream << static_cast<quint8>(Stop);
            break;
        }
        default:
            Q_UNREACHABLE();
    }
    socket->write(message);
    socket->flush();
    tasks.append(task);
}

void Node::kick()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream << static_cast<quint8>(Disconnect) << QByteArray("Kicked by owner");
    socket->write(message);
    socket->flush();
    QObject::disconnect(socket, SIGNAL(disconnected()), nullptr, nullptr);
    QObject::disconnect(socket, SIGNAL(readyRead()), nullptr, nullptr);
    socket->close();
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
                    throw QString("Unknown message type");
                message.type = static_cast<MessageType>(t);
                message.toParse = Message::MessagePart::Length;
                switch(message.type) {
                    case Accept: {
                        if(!processMessage())
                            return;
                        if(socket->bytesAvailable())
                            QTimer::singleShot(0, this, SLOT(readyRead()));
                        break;
                    }
                    case Start: {
                        message.length = sizeof(quint64);
                        message.toParse = Message::MessagePart::Body;
                        if(socket->bytesAvailable())
                            readyRead();
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
                            throw QString("Unknown node status");
                        message.body.append(s);
                        if(!processMessage())
                            return;
                        if(socket->bytesAvailable())
                            QTimer::singleShot(0, this, SLOT(readyRead()));
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
                                throw QString("Invalid message length");
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
                quint32 forBody = message.length - buffer.length();
                if(available >= forBody) {
                    buffer.append(socket->read(forBody));
                    message.body.append(buffer);
                    if(!processMessage())
                        return;
                    if(socket->bytesAvailable())
                        QTimer::singleShot(0, this, SLOT(readyRead()));
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

void Node::disconnected()
{
    emit left("Connection interrupted");
}

void Node::Message::reset()
{
    toParse = Type;
    type = None;
    length = 0;
    body.clear();
}

int Node::taskIndex(Task::Type type)
{
    for(qint32 i = 0; i < tasks.length(); i++)
        if(tasks[i]->getType() == type)
            return i;
    return -1;
}

bool Node::processMessage()
{
    if(this->message.type == Disconnect) {
        emit left(this->message.body);
        QObject::disconnect();
        return false;
    }
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    bool processFurther = true;
    if(this->message.type == Status) {
        int index = taskIndex(Task::GetStatus);
        status = static_cast<NodeStatus>(this->message.body.at(0));
        if(index == -1) {
            stream << static_cast<quint8>(Accept);
            emit statusChanged();
        } else
            emit taskFinished(tasks.takeAt(index));
    } else {
        try {
            switch(status) {
                case Connecting: {
                    QRegularExpressionMatch match = QRegularExpression("^(\\d+\\.\\d+\\.\\d+);((?:[a-z_][a-z0-9_]*,?)*)").match(this->message.body);
                    if(match.hasMatch()) {
                        version = match.captured(1);
                        modules = match.captured(2).split(',', QString::SkipEmptyParts);
                        modules.sort();
                        status = Idle;
                        stream << static_cast<quint8>(Accept) << name;
                        emit joined();
                    } else {
                        QByteArray reason("Invalid join message");
                        stream << static_cast<quint8>(Reject) << reason;
                        emit joinError(socket->peerAddress().toString(), reason);
                        throw 0;
                    }
                    break;
                }
                case Idle: {
                    switch(this->message.type) {
                        case Accept:
                        case Reject: {
                            int index = taskIndex(Task::Assign);
                            if(index == -1)
                                throw 1;
                            else {
                                Task* t = tasks.takeAt(index);
                                if(this->message.type == Accept) {
                                    t->finish(0);
                                    status = ReadyToStart;
                                } else
                                    t->finish(1, this->message.body);
                                emit taskFinished(t);
                            }
                            break;
                        }
                        default: {
                            throw 1;
                        }
                    }
                    break;
                }
                case ReadyToStart: {
                    switch(this->message.type) {
                        case Start:
                        case Accept:
                        case Reject: {
                            int index;
                            if((index = taskIndex(Task::Start)) != -1)  {
                                Task* t = tasks.takeAt(index);
                                if(this->message.type == Start) {
                                    QDataStream s(this->message.body);
                                    s >> jobStartedAt;
                                    t->finish(0);
                                    status = Working;
                                } else
                                    t->finish(1, this->message.body);
                                emit taskFinished(t);
                            } else if((index = taskIndex(Task::Assign)) != -1) {
                                AssignTask* t = static_cast<AssignTask*>(tasks.takeAt(index));
                                if(this->message.type == Accept) {
                                    t->finish(0);
                                    status = t->codeBytes.length() ? ReadyToStart : Idle;
                                } else
                                    t->finish(1, t->codeBytes.length() ? "Unable to assign new job" : "Unable to remove job");
                                emit taskFinished(t);
                            } else
                                throw 1;
                            break;
                        }
                        default: {
                            throw 1;
                        }
                    }
                    break;
                }
                case Working: {
                    switch(this->message.type) {
                        case Finished: {
                            if(this->message.body[8] == ';') {
                                QDataStream s(this->message.body);
                                s >> jobFinishedAt;
                                jobFinishedAtLocal = QDateTime::currentMSecsSinceEpoch();
                                stream << static_cast<quint8>(Accept);
                                emit jobFinished(this->message.body.right(this->message.body.length() - 9));
                            } else {
                                QByteArray reason("Invalid job finished message");
                                stream << static_cast<quint8>(Reject) << reason;
                                emit unexpectedMessage(reason);
                            }
                            status = Idle;
                            break;
                        }
                        default: {
                            throw 1;
                        }
                    }
                    break;
                }
            }
        } catch(int code) {
            if(code) {
                QByteArray reason(QString("Node sent \'%1\' while \'%2\'").arg(typeText[this->message.type], statusText[status]).toLatin1());
                stream << static_cast<quint8>(Reject) << reason;
                emit unexpectedMessage(reason);
            } else
                processFurther = false;
        }
    }
    socket->write(message);
    socket->flush();
    this->message.reset();
    buffer.clear();
    if(!processFurther) {
        QObject::disconnect(socket, SIGNAL(disconnected()), nullptr, nullptr);
        QObject::disconnect(socket, SIGNAL(readyRead()), nullptr, nullptr);
        socket->close();
    }
    return processFurther;
}
