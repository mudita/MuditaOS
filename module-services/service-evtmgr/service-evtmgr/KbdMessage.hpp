// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"

#include <common_data/RawKey.hpp>

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
