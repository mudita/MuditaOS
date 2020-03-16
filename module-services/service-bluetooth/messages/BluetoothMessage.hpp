#pragma once

#include "Service/Message.hpp"
#include "MessageType.hpp"

class BluetoothMessage : public sys::DataMessage
{
  public:
    enum Request
    {
        None,
        Start,
        Scan,
        PAN,
        Visible,
    };
    enum Request req = Request::None;
    BluetoothMessage(enum Request req = None) : sys::DataMessage(MessageType::BluetoothRequest), req(req){};
    virtual ~BluetoothMessage() = default;
};
