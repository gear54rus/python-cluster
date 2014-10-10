#include <QMutexLocker>
#include <QThread>

#include "core.h"

Core::Core() :
    QObject(nullptr),
    nameList(nullptr),
    server(this),
    nextId(1)
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
    QString name = nameList->takeFirst();
    nameList->append(name);
    Node* node = new Node(server.nextPendingConnection(), nextId++, name);
    QObject::connect(node, SIGNAL(taskFinished(Task*)), this, SLOT(nodeTaskFinished(Task*)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(malformedMessage(QString)), this, SLOT(nodeMalformedMessage(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(unexpectedMessage(QString)), this, SLOT(nodeUnexpectedMessage(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(joinError(QString)), this, SLOT(nodeJoinError(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(joined()), this, SLOT(nodeJoined()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(left(QString)), this, SLOT(nodeLeft(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(statusChanged()), this, SLOT(nodeStatusChanged()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(jobFinished(QByteArray)), this, SLOT(nodeJobFinished(QByteArray)), Qt::QueuedConnection);
    nodes.append(node);
}

void Core::nodeTaskFinished(Task* task)
{
    delete task;
}

void Core::nodeMalformedMessage(QString reason)
{
}

void Core::nodeUnexpectedMessage(QString reason)
{
}

void Core::nodeJoinError(QString reason)
{
    Node* node = static_cast<Node*>(QObject::sender());
    nodes.removeAt(nodes.indexOf(node));
    emit newEvent(new JoinErrorEvent(node->getAddress(), reason));
    node->deleteLater();
}

void Core::nodeJoined()
{
    emit newEvent(new NodeJoinedEvent(nodes.indexOf(static_cast<Node*>(QObject::sender()))));
}
void Core::nodeLeft(QString reason)
{
    Node* node = static_cast<Node*>(QObject::sender());
    quint32 i = nodes.indexOf(node);
    nodes.removeAt(i);
    emit newEvent(new NodeLeftEvent(i, node->getId(), node->getName(), reason));
    node->deleteLater();
}

void Core::nodeStatusChanged()
{
}

void Core::nodeJobFinished(QByteArray output)
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
