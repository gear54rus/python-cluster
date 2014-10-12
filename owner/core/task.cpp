#include <QEvent>

#include "task.h"

Task::Task()
{
    type = None;
    status = NotStarted;
    code = 0;
}

void Task::start()
{
    status = Started;
}

void Task::finish(const quint8 code, const QString& message)
{
    this->code = code;
    this->message = message;
    status = Finished;
}

ListenTask::ListenTask(const QHostAddress& address, quint16 port)
{
    this->address = address;
    this->port = port;
    type = Listen;
}

AssignTask::AssignTask(quint32 nodeIndex, const QByteArray& input, const QByteArray& code)
{
    this->nodeIndex = nodeIndex;
    this->input = input;
    this->code = code;
    type = Assign;
}

StartTask::StartTask(quint32 nodeIndex)
{
    this->nodeIndex = nodeIndex;
    type = Start;
}

StopTask::StopTask(quint32 nodeIndex)
{
    this->nodeIndex = nodeIndex;
    type = Stop;
}

KickTask::KickTask(quint32 nodeIndex)
{
    this->nodeIndex = nodeIndex;
    type = Kick;
}

GetStatusTask::GetStatusTask(quint32 nodeIndex)
{
    this->nodeIndex = nodeIndex;
    type = GetStatus;
}
