// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional> // for function
#include <SwitchData.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    struct BellFinishedCallbackWindowSwitchData : public gui::SwitchData
    {
      public:
        struct Factory
        {
            static std::unique_ptr<BellFinishedCallbackWindowSwitchData> create(const UTF8 &icon,
                                                                                const UTF8 &text,
                                                                                std::function<void()> finishCallback)
            {
                return std::unique_ptr<BellFinishedCallbackWindowSwitchData>(
                    new BellFinishedCallbackWindowSwitchData(icon, text, finishCallback));
            }
        };

        UTF8 icon{};
        UTF8 text{};
        std::function<void()> finishCallback;

      private:
        BellFinishedCallbackWindowSwitchData() = default;

        BellFinishedCallbackWindowSwitchData(const UTF8 &icon, const UTF8 &text, std::function<void()> finishCallback)
            : icon{icon}, text{text}, finishCallback{finishCallback}
        {}
    };
} // namespace gui
