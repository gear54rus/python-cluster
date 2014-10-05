#ifndef EVENT_H
#define EVENT_H

class Event {
public:
    enum Type {
        None = 0,
        UnexpectedMessage,
        JoinError,
        NodeJoined,
        NodeLeft,
        NodeStatusChanged,
        NodeKicked,
        JobFinished
    };

    Event();
};

#endif // EVENT_H
