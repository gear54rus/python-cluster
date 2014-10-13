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
    switch(task->getType()) {
        case Task::Listen: {
            listen(static_cast<ListenTask*>(task));
            break;
        }
        case Task::Assign: {
            auto t = static_cast<AssignTask*>(task);
            nodes[t->nodeIndex]->addTask(t);
            break;
        }
        case Task::GetStatus: {
            auto t = static_cast<GetStatusTask*>(task);
            nodes[t->nodeIndex]->addTask(t);
            break;
        }
        case Task::Kick: {
            auto t = static_cast<KickTask*>(task);
            Node* node = nodes.takeAt(t->nodeIndex);
            t->nodeId = node->getId();
            t->nodeName = node->getName();
            node->kick();
            node->deleteLater();
            t->finish(0);
            emit taskFinished(t);
            break;
        }
        case Task::Start: {
            auto t = static_cast<StartTask*>(task);
            nodes[t->nodeIndex]->addTask(t);
            break;
        }
        case Task::Stop: {
            auto t = static_cast<StopTask*>(task);
            nodes[t->nodeIndex]->addTask(t);
            break;
        }
        default: {
            task->finish(1, "Unknown task type");
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
    QObject::connect(node, SIGNAL(joinError(QString, QString)), this, SLOT(nodeJoinError(QString, QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(joined()), this, SLOT(nodeJoined()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(left(QString)), this, SLOT(nodeLeft(QString)), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(statusChanged()), this, SLOT(nodeStatusChanged()), Qt::QueuedConnection);
    QObject::connect(node, SIGNAL(jobFinished(QByteArray)), this, SLOT(nodeJobFinished(QByteArray)), Qt::QueuedConnection);
    nodes.append(node);
}

void Core::nodeTaskFinished(Task* task)
{
    emit taskFinished(task); // 10/10 perfect design
}

void Core::nodeMalformedMessage(QString reason)
{
    Node* node = static_cast<Node*>(QObject::sender());
    quint32 i = nodes.indexOf(node);
    nodes.removeAt(i);
    emit newEvent(new MalformedMessageEvent(i, node->getId(), node->getName(), reason));
    node->deleteLater();
}

void Core::nodeUnexpectedMessage(QString reason)
{
    emit newEvent(new UnexpectedMessageEvent(nodes.indexOf(static_cast<Node*>(QObject::sender())), reason));
}

void Core::nodeJoinError(QString address, QString reason)
{
    Node* node = static_cast<Node*>(QObject::sender());
    nodes.removeAt(nodes.indexOf(node));
    emit newEvent(new JoinErrorEvent(address, reason));
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
    emit newEvent(new NodeStatusChangedEvent(nodes.indexOf(static_cast<Node*>(QObject::sender()))));
}

void Core::nodeJobFinished(QByteArray output)
{
    emit newEvent(new JobFinishedEvent(nodes.indexOf(static_cast<Node*>(QObject::sender())), output));
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
