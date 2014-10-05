#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QStringList>
#include <QTcpSocket>
#include <QTcpServer>

#include "event.h"
#include "task.h"
#include "../network/node.h"

class Core : public QObject {
    Q_OBJECT
public:
    explicit Core();
    void setNameList(QStringList* list);
signals:
    void taskFinished(Task* task);
    void newEvent(Event*);

public slots:

private slots:
    void newTask(Task* task);
    void newNode();
private:
    QMutex m;
    QStringList* nameList;
    QTcpServer server;
    QList<Node*> nodes;
    void listen(ListenTask* task);
};

#endif // CORE_H
