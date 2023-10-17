// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

namespace gui
{
    class FinishedWindowMessageData : public gui::SwitchData
    {
      public:
        explicit FinishedWindowMessageData(std::string message) : _message(std::move(message))
        {}

        [[nodiscard]] std::string getMessage() const
        {
            return _message;
        }

      private:
        std::string _message{};
    };
}; // namespace gui
