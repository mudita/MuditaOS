// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionSetting.hpp>

namespace gui
{
    class SpinBoxOptionSettings : public option::Base
    {
      public:
        SpinBoxOptionSettings(UTF8 text,
                              uint8_t value,
                              uint8_t maxValue,
                              std::function<bool(uint8_t)> updateCallback,
                              std::function<bool(Item &)> focusChangedCallback = nullptr,
                              bool indent                                      = false);

        [[nodiscard]] auto build() const -> ListItem * override;

      private:
        std::function<bool(uint8_t)> updateCallback;
        std::uint8_t maxValue;
        std::uint8_t value;
        UTF8 text;
        bool indent = false;
    };
} // namespace gui
