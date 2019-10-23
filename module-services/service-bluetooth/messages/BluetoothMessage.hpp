#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"

class BluetoothMessage : public sys::DataMessage {
public:
    enum Request {
        None,
        Start,
        Scan,
        PAN,
        Visible,
    };
    enum Request req = Request::None;
    BluetoothMessage(MessageType messageType, enum Request req = None) : sys::DataMessage(static_cast<uint32_t>(messageType)), req(req) {};
    virtual ~BluetoothMessage() = default;
};
