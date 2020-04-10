#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>

namespace sevm
{
    struct Message : public sys::DataMessage
    {
        Message(MessageType messageType);
    };
}; // namespace sevm
