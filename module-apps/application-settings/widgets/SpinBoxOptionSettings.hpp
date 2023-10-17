// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionSetting.hpp>

namespace gui
{
    class SpinBoxOptionSettings : public option::Base
    {
      public:
        SpinBoxOptionSettings(const UTF8 &text,
                              const UTF8 &textFocused,
                              std::uint8_t value,
                              std::uint8_t maxValue,
                              std::function<bool(std::uint8_t)> updateCallback,
                              std::function<void(const UTF8 &text)> navBarTemporaryMode,
                              std::function<void()> navBarRestoreFromTemporaryMode,
                              bool indent = false);

        [[nodiscard]] auto build() const -> ListItem * override;

      private:
        std::function<bool(uint8_t)> updateCallback;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;
        std::uint8_t maxValue;
        std::uint8_t value;
        UTF8 text;
        UTF8 textFocused;
        bool indent = false;
    };
} // namespace gui
