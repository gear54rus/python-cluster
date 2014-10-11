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
    UnexpectedMessageEvent(const quint32 index, const QString& reason);
    quint32 index;
    QString reason;
};

class MalformedMessageEvent : public Event {
public:
    MalformedMessageEvent(const quint32 index, const QString& reason);
    quint32 index;
    QString reason;
};

class JoinErrorEvent : public Event {
public:
    JoinErrorEvent(const QString& address, const QString& reason);
    QString address, reason;
};

class NodeJoinedEvent : public Event {
public:
    NodeJoinedEvent(const quint32 index);
    quint32 index;
};

class NodeLeftEvent : public Event {
public:
    NodeLeftEvent(const quint32 index, const quint32 id, const QString& name, const QString& leaveDesctiption);
    quint32 index, id;
    QString name, leaveDesctiption;
};

class NodeStatusChangedEvent : public Event {
public:
    NodeStatusChangedEvent(quint32 index);
    quint32 index;
};

class JobFinishedEvent : public Event {
public:
    JobFinishedEvent(const quint32 index, const QByteArray& output);
    quint32 index;
    QByteArray output;
};

#endif // EVENT_H
