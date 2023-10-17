// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

namespace service::eink
{
    class EinkModeMessage : public EinkMessage
    {
      public:
        enum class Mode
        {
            Normal,
            Invert
        };

        explicit EinkModeMessage(Mode mode) : mode(mode)
        {}

        [[nodiscard]] auto getMode() const noexcept
        {
            return mode;
        }

      private:
        Mode mode = Mode::Normal;
    };

    class EinkModeResponse : public sys::ResponseMessage
    {
        using sys::ResponseMessage::ResponseMessage;
    };
} // namespace service::eink
