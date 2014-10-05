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

AssignTask::AssignTask(quint8 nodeID, const QByteArray& input, const QString& code)
{
    this->nodeID = nodeID;
    this->input = input;
    this->code = code;
    type = Assign;
}

StartTask::StartTask(quint8 nodeID)
{
    this->nodeID = nodeID;
    type = Start;
}

StopTask::StopTask(quint8 nodeID) :
    StartTask(nodeID)
{
    type = Stop;
}

KickTask::KickTask(quint8 nodeID) :
    StartTask(nodeID)
{
    type = Kick;
}
