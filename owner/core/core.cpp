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
    nodes.append(new Node(server.nextPendingConnection()));
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
