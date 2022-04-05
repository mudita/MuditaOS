// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
    class TimeSetSpinnerVertical;
    class TimeSetFmtSpinner;
    class SnoozeTimer;
    class BellBattery;
    class DuoHBox;
    class Icon;
    class Item;

    namespace battery_vertical
    {
        constexpr auto chargingLevelHideBot    = 100;
        constexpr auto dischargingLevelShowTop = 20;
    }; // namespace battery_vertical

    enum class ScreenMode
    {
        Main,
        AlarmEdit,
        AlarmActivatedDeactivated,
    };

    class HomeScreenLayoutVertical : public BaseHomeScreenLayoutProvider, public LayoutVertical
    {
      public:
        HomeScreenLayoutVertical(std::string name);

        virtual auto setViewState(app::home_screen::ViewState state) -> void override;
        virtual auto setTime(std::time_t newTime) -> void override;
        auto setTextDescription(const UTF8 &desc) -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;

        auto getSnoozeTimer() -> SnoozeTimer * override;
        auto getLayout() -> Item * override;

      protected:
        auto setScreenMode(ScreenMode mode) -> void;
        virtual bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext);
    };
}; // namespace gui
