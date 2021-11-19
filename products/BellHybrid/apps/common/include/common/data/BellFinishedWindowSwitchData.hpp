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
        struct Factory
        {
            static std::unique_ptr<BellFinishedWindowData> create(
                const UTF8 &icon,
                const std::string &windowToReturn,
                const UTF8 &text                   = "",
                bool closeApplication              = false,
                const std::chrono::seconds timeout = std::chrono::seconds::zero())
            {
                return std::unique_ptr<BellFinishedWindowData>(
                    new BellFinishedWindowData(icon, windowToReturn, text, closeApplication, timeout));
            }
        };

        UTF8 icon{};
        std::string windowToReturn;
        UTF8 text{};
        bool closeApplication;
        std::chrono::seconds timeout;

      private:
        BellFinishedWindowData() = default;

        BellFinishedWindowData(const UTF8 &icon,
                               const std::string &windowToReturn,
                               const UTF8 &text                   = "",
                               bool closeApplication              = false,
                               const std::chrono::seconds timeout = std::chrono::seconds::zero())
            : icon{icon}, windowToReturn{windowToReturn}, text{text}, closeApplication{closeApplication}, timeout{
                                                                                                              timeout}
        {}
    };
} // namespace gui
