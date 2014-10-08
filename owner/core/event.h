#ifndef EVENT_H
#define EVENT_H

#include <QString>

class Event {
public:
    enum Type {
        None = 0,
        UnexpectedMessage,
        MalformedMessage,
        JoinError,
        NodeJoined,
        NodeLeft,
        NodeStatusChanged,
        JobFinished
    };

    Event();
    virtual ~Event() {}
    inline Type getType() const {
        return type;
    }
protected:
    Type type;
};

class UnexpectedMessageEvent : public Event {
public:
    UnexpectedMessageEvent(const QString& expectedGot);
    QString expectedGot;
};

class MalformedMessageEvent : public Event {
public:
    MalformedMessageEvent(const QString& reason);
    QString reason;
};

class JoinErrorEvent : public Event {
public:
    JoinErrorEvent(const QString& address, const QString& reason);
    QString address;
    QString reason;
};

class NodeJoinedEvent : public Event {
public:
    NodeJoinedEvent(quint32 id);
    quint32 id;
};

class NodeLeftEvent : public Event {
public:
    NodeLeftEvent(const QString& leaveDesctiption);
    QString leaveDesctiption;
};

class NodeStatusChangedEvent : public NodeJoinedEvent {
public:
    NodeStatusChangedEvent(quint32 id);
};

class JobFinishedEvent : public NodeJoinedEvent {
public:
    JobFinishedEvent(quint32 id, const QByteArray& output);
    QByteArray output;
};

#endif // EVENT_H
