// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    struct BellFinishedWindowData : public gui::SwitchData
    {
      public:
        struct Factory
        {
            static std::unique_ptr<BellFinishedWindowData> create(const UTF8 &icon,
                                                                  const std::string &windowToReturn)
            {
                return std::unique_ptr<BellFinishedWindowData>(new BellFinishedWindowData(icon, windowToReturn));
            }
        };

        UTF8 icon{};
        std::string windowToReturn;

      private:
        BellFinishedWindowData() = default;

        BellFinishedWindowData(const UTF8 &icon, const std::string &windowToReturn)
            : icon{icon}, windowToReturn{windowToReturn}
        {}
    };
} // namespace gui
