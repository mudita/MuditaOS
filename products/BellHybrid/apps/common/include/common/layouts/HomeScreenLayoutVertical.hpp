// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BaseHomeScreenLayoutProvider.hpp"
#include "widgets/LayoutVertical.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <module-gui/gui/widgets/Item.hpp>

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmIcon;
    class AlarmSetSpinner;
    class ClockVertical;
    class TimeSetFmtSpinner;
    class SnoozeTimer;
    class BellBattery;
    class Icon;
    class Item;

    class HomeScreenLayoutVertical : public BaseHomeScreenLayoutProvider, public LayoutVertical
    {
      public:
        enum class ScreenMode
        {
            Main,
            AlarmEdit,
            AlarmActivatedDeactivated,
        };

        explicit HomeScreenLayoutVertical(std::string name);

        auto setViewState(app::home_screen::ViewState state) -> void override;
        auto setTime(std::time_t newTime) -> void override;
        auto setTextDescription(const UTF8 &desc) -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        [[nodiscard]] auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;
        auto updateUsbStatus(bool isConnected) -> void override;

        auto getSnoozeTimer() -> SnoozeTimer * override;
        auto getLayout() -> Item * override;

      protected:
        auto setScreenMode(ScreenMode mode) -> void;
        virtual auto isBatteryVisibilityAllowed(const Store::Battery &batteryContext) -> bool;
        virtual auto isAlarmTimeVisibilityAllowed() -> bool;
    };
} // namespace gui
