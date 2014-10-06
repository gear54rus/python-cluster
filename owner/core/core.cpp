#include <QMutexLocker>
#include <QThread>

#include "core.h"

Core::Core() :
    QObject(nullptr),
    nameList(nullptr),
    server(this)
{
    QObject::moveToThread(new QThread(nullptr));
    thread()->start();
    QObject::connect(&server, SIGNAL(newConnection()), this, SLOT(newNode()));
}

void Core::setNameList(QStringList* list)
{
    QMutexLocker l(&m);
    nameList = list;
    if(!nameList->length())
        nameList->append("NoName");
}

void Core::newTask(Task* task)
{
    QString unknownType("Unknown type");
    switch(task->getType()) {
        case Task::Listen: {
            if(auto t = dynamic_cast<ListenTask*>(task))
                listen(t);
            else {
                task->finish(1, unknownType);
                emit taskFinished(task);
            }
            break;
        }
        default: {
            task->finish(1, unknownType);
            emit taskFinished(task);
        }
    }
}

void Core::newNode()
{
    Node* node = new Node(server.nextPendingConnection());
    QObject::connect(node, SIGNAL(taskFinished(Task*)), this, SLOT(nodeTaskFinished(Task*)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(malformedMessage(QString)), this, SLOT(nodeMalformedMessage(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(unexpectedMessage(QString)), this, SLOT(nodeUnexpectedMessage(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(joinError(QString)), this, SLOT(nodeJoinError(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(joined()), this, SLOT(nodeJoined()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(left()), this, SLOT(nodeLeft()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(statusChanged()), this, SLOT(nodeStatusChanged()), Qt::QueuedConnection);
    nodes.append(node);
}

void Core::nodeTaskFinished(Task*)
{
}

void Core::nodeMalformedMessage(QString reason)
{
}

void Core::nodeUnexpectedMessage(QString reason)
{
}

void Core::nodeJoinError(QString reason)
{
}

void Core::nodeJoined()
{
}
void Core::nodeLeft()
{
}

void Core::nodeStatusChanged()
{
}

void Core::listen(ListenTask* task)
{
    if(server.isListening())
        server.close();
    if(server.listen(task->address, task->port))
        task->finish(0);
    else
        task->finish(1, server.errorString());
    emit taskFinished(task);
}
