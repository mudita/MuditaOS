// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <utf8/UTF8.hpp>

#include <chrono>

namespace gui
{
    struct BellFinishedWindowData : public gui::SwitchData
    {
      public:
        enum class ExitBehaviour
        {
            SwitchWindow,
            CloseApplication,
            ReturnToHomescreen,
        };

        struct Factory
        {
            static std::unique_ptr<BellFinishedWindowData> create(const UTF8 &icon,
                                                                  const std::string &windowToReturn,
                                                                  const UTF8 &text,
                                                                  ExitBehaviour exitBehaviour,
                                                                  std::chrono::seconds timeout)
            {
                return std::unique_ptr<BellFinishedWindowData>(
                    new BellFinishedWindowData(icon, windowToReturn, text, exitBehaviour, timeout));
            }

            static std::unique_ptr<BellFinishedWindowData> create(const UTF8 &icon, const std::string &windowToReturn)
            {
                return create(icon, windowToReturn, "", ExitBehaviour::SwitchWindow, std::chrono::seconds::zero());
            }
        };

        UTF8 icon{};
        std::string windowToReturn;
        UTF8 text{};
        ExitBehaviour exitBehaviour;
        std::chrono::seconds timeout;

      private:
        BellFinishedWindowData() = default;

        BellFinishedWindowData(const UTF8 &icon,
                               const std::string &windowToReturn,
                               const UTF8 &text                   = "",
                               const ExitBehaviour exitBehaviour  = ExitBehaviour::SwitchWindow,
                               const std::chrono::seconds timeout = std::chrono::seconds::zero())
            : icon{icon}, windowToReturn{windowToReturn}, text{text}, exitBehaviour{exitBehaviour}, timeout{timeout}
        {}
    };
} // namespace gui
