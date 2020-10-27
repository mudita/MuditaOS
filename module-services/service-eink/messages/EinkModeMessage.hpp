// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

namespace seink
{
    class EinkModeMessage : public EinkMessage
    {
      public:
        enum Mode
        {
            Normal,
            Invert
        };
        EinkModeMessage(Mode mode) : EinkMessage(MessageType::EinkMessage), mode(mode)
        {}

        [[nodiscard]] auto getMode() noexcept
        {
            return mode;
        }

      private:
        Mode mode = Normal;
    };
} // namespace seink
