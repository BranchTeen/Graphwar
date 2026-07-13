#pragma once
#include <QObject>
#include "EventBus.h"

template<typename Sender, typename Receiver>
class EventForwarder {
public:
    EventForwarder(Sender *sender, Receiver *receiver)
        : m_sender(sender), m_receiver(receiver) {}

private:
    Sender *m_sender = nullptr;
    Receiver *m_receiver = nullptr;
};
