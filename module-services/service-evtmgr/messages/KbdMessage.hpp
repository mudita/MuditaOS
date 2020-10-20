// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
