#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QList>
#include <QString>
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
    static QStringList typeText;
    static QStringList statusText;
    explicit Node(QTcpSocket* socket, quint32 id, QString name);
    inline quint32 getId() const {
        return id;
    }
    inline QByteArray getName() const {
        return name;
    }
    inline QString getAddress() const {
        return socket->peerAddress().toString();
    }
    inline QString getVersion() const {
        return version;
    }
    inline QStringList getModules() const {
        return modules;
    }
    inline QString getStatus() const {
        return statusText[status];
    }
    inline bool getReadyToStart() const {
        return status == ReadyToStart;
    }

    void addTask(Task* task);
    void kick();
    ~Node();
    quint64 jobStartedAt, jobFinishedAt, jobStartedAtLocal, jobFinishedAtLocal;
signals:
    void taskFinished(Task* task);
    void malformedMessage(QString reason);
    void unexpectedMessage(QString reason);
    void joinError(QString address, QString reason);
    void joined();
    void left(QString reason);
    void statusChanged();
    void jobFinished(QByteArray output);
public slots:

private slots:
    void readyRead();
    void disconnected();

private:
    NodeStatus status;
    quint32 id;
    QByteArray name;
    QList<Task*> tasks;
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
    int taskIndex(Task::Type type);
    bool processMessage();
};

#endif // NODE_H
