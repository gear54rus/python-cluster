#include "event.h"

Event::Event()
{
    type = None;
}

UnexpectedMessageEvent::UnexpectedMessageEvent(const QString& reason)
{
    type = UnexpectedMessage;
    this->reason = reason;
}

MalformedMessageEvent::MalformedMessageEvent(const QString& reason)
{
    type = MalformedMessage;
    this->reason = reason;
}

JoinErrorEvent::JoinErrorEvent(const QString& address, const QString& reason)
{
    type = JoinError;
    this->address = address;
    this->reason = reason;
}

NodeJoinedEvent::NodeJoinedEvent(quint32 id)
{
    type = NodeJoined;
    this->id = id;
}

NodeLeftEvent::NodeLeftEvent(const QString& leaveDesctiption)
{
    type = NodeLeft;
    this->leaveDesctiption = leaveDesctiption;
}

NodeStatusChangedEvent::NodeStatusChangedEvent(quint32 id) :
    NodeJoinedEvent(id)
{
    type = NodeStatusChanged;
}

JobFinishedEvent::JobFinishedEvent(quint32 id, const QByteArray& output) :
    NodeJoinedEvent(id)
{
    type = JobFinished;
    this->output = output;
}
