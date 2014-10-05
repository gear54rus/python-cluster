#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQueue>
#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>

#include "../global.h"
#include "../core/task.h"

class Node : public QObject {
    Q_OBJECT
public:
    enum MessageType {
        None = 0x0,
        Accept = 0x1,
        Reject = 0x2,
        Join = 0x3,
        Leave = 0x4,
        Status = 0x5,
        Job = 0x6,
        Start = 0x7,
        Stop = 0x8,
        Finished = 0x9,
        Disconnect = 0xA
    };
    enum NodeStatus {
        Connecting = 0,
        Idle,
        ReadyToStart,
        Working
    };

    explicit Node(QTcpSocket* socket);
    void addTask(Task* task);
    ~Node();
signals:
    void taskFinished(Task* task);
    void malformedMessage(QString reason);
    void unexpectedMessage(QString reason);
    void joinError(QString reason);
    void joined();
    void left();
    void statusChanged();
public slots:

private slots:
    void readyRead();

private:
    NodeStatus status;
    QQueue<Task*> tasks;
    QByteArray buffer;
    QTcpSocket* socket;
    struct Message {
        MessageType type;
        quint32 length;
        QByteArray body;
        void reset();
    };
    Message message;
    void processMessage();
};

#endif // NODE_H
