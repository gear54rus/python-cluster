#include "event.h"

Event::Event()
{
    type = None;
}

UnexpectedMessageEvent::UnexpectedMessageEvent(quint32 index, const QString& reason)
{
    type = UnexpectedMessage;
    this->index = index;
    this->reason = reason;
}

MalformedMessageEvent::MalformedMessageEvent(const quint32 index, const quint32 id, const QString& name, const QString& reason)
{
    type = MalformedMessage;
    this->index = index;
    this->id = id;
    this->name = name;
    this->reason = reason;
}

JoinErrorEvent::JoinErrorEvent(const QString& address, const QString& reason)
{
    type = JoinError;
    this->address = address;
    this->reason = reason;
}

NodeJoinedEvent::NodeJoinedEvent(const quint32 index)
{
    type = NodeJoined;
    this->index = index;
}

NodeLeftEvent::NodeLeftEvent(const quint32 index, const quint32 id, const QString& name, const QString& leaveDesctiption)
{
    type = NodeLeft;
    this->index = index;
    this->id = id;
    this->name = name;
    this->leaveDesctiption = leaveDesctiption;
}

NodeStatusChangedEvent::NodeStatusChangedEvent(const quint32 index)
{
    type = NodeStatusChanged;
    this->index = index;
}

JobFinishedEvent::JobFinishedEvent(const quint32 index, const QByteArray& output)
{
    type = JobFinished;
    this->index = index;
    this->output = output;
}
