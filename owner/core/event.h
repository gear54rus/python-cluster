#ifndef EVENT_H
#define EVENT_H

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
};

#endif // EVENT_H
