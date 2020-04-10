#pragma once

#include "Message.hpp"
#include <common_data/RawKey.hpp>

namespace sevm
{
    class KbdMessage : public Message
    {
      public:
        KbdMessage() : Message(MessageType::KBDKeyEvent)
        {
            type = Type::Data;
        }
        RawKey key = {};
    };
} // namespace sevm
