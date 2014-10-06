#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQueue>
#include <QStringList>
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
        Status = 0x4,
        Job = 0x5,
        Start = 0x6,
        Stop = 0x7,
        Finished = 0x8,
        Disconnect = 0x9
    };
    enum NodeStatus {
        Connecting = 0,
        Idle,
        ReadyToStart,
        Working
    };

    explicit Node(QTcpSocket* socket);
    void addTask(Task* task);
    void kick();
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
        enum MessagePart {
            Type = 0,
            Length,
            Body
        };
        MessagePart toParse;
        MessageType type;
        quint32 length;
        QByteArray body;
        void reset();
    };
    QString version;
    QStringList modules;
    Message message;
    int taskIndex(Task* task);
    bool processMessage();
};

#endif // NODE_H
