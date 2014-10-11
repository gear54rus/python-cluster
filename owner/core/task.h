#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QHostAddress>

class Task {
public:
    enum Type {
        None = 0,
        Listen,
        GetStatus,
        Assign,
        Start,
        Stop,
        Kick
    };
    enum Status {
        NotStarted = 0,
        Started,
        Finished
    };
    Task();
    virtual ~Task() {}
    inline Type getType() const {
        return type;
    }
    inline Status getStatus() const {
        return status;
    }
    inline quint8 getCode() const {
        return code;
    }
    inline QString getMessage() const {
        return message;
    }
    void start();
    void finish(const quint8 code, const QString& message = QString());

protected:
    Type type;
    Status status;
    quint8 code;
    QString message;
};

class ListenTask : public Task {
public:
    ListenTask(const QHostAddress& address, quint16 port);
    QHostAddress address;
    quint16 port;
};

class AssignTask : public Task {
public:
    AssignTask(quint8 nodeIndex, const QByteArray& input, const QByteArray& code);
    quint8 nodeIndex;
    QByteArray input, code;
};

class StartTask : public Task {
public:
    StartTask(quint8 nodeIndex);
    quint8 nodeIndex;
};

class StopTask : public StartTask {
public:
    StopTask(quint8 nodeIndex);
};

class KickTask : public StartTask {
public:
    KickTask(quint8 nodeIndex);
};

class GetStatusTask : public StartTask {
public:
    GetStatusTask(quint8 nodeIndex);
};

#endif // TASK_H
