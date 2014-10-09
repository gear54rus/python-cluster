#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QMutex>
#include <QMap>
#include <QString>
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
    inline const QMap<quint32, Node*>* getNodeMap() {
        return &nodes;
    }

signals:
    void taskFinished(Task* task);
    void newEvent(Event*);

public slots:

private slots:
    void newTask(Task* task);
    void newNode();
    void nodeTaskFinished(Task* task);
    void nodeMalformedMessage(QString reason);
    void nodeUnexpectedMessage(QString reason);
    void nodeJoinError(QString reason);
    void nodeJoined();
    void nodeLeft(QString reason);
    void nodeStatusChanged();
    void nodeJobFinished(QByteArray output);

private:
    QMutex m;
    QStringList* nameList;
    QTcpServer server;
    quint32 nextNodeId;
    QMap<quint32, Node*> nodes;
    void listen(ListenTask* task);
};

#endif // CORE_H
