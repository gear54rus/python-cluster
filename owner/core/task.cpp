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
    status = Finished;
    this->code = code;
    this->message = message;
}

ListenTask::ListenTask(const QHostAddress& address, quint16 port)
{
    this->address = address;
    this->port = port;
    type = Listen;
}
