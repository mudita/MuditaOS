// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Service/Message.hpp"

#include <hal/key_input/RawKey.hpp>

namespace sevm
{
    class KbdMessage : public sys::DataMessage
    {
      public:
        KbdMessage() : DataMessage(MessageType::KBDKeyEvent)
        {}
        RawKey key = {};
    };
} // namespace sevm
