// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseHomeScreenLayoutProvider.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace style::homescreen_classic
{
    constexpr inline auto bottom_box_w = 390U;
    constexpr inline auto bottom_box_h = 102U;
} // namespace style::homescreen_classic
namespace gui
{

    class BellBaseLayout;
    class TextFixedSize;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;
    class SnoozeTimer;
    class BellBattery;
    class DuoHBox;

    enum class LayoutClassicVersion
    {
        Temperature,
        AmPm,
        Battery
    };

    enum class HeaderViewMode
    {
        Empty,
        AlarmIcon,
        AlarmIconAndTime,
        SnoozeIcon,
        SnoozeIconAndTime,
    };

    namespace battery
    {
        constexpr auto chargingLevelHideBot    = 100;
        constexpr auto dischargingLevelShowTop = 20;
    }; // namespace battery

    class HomeScreenLayoutClassic : public BaseHomeScreenLayoutProvider, BellBaseLayout
    {
      public:
        HomeScreenLayoutClassic(std::string name);

        virtual auto setViewState(app::home_screen::ViewState state) -> void override;
        virtual auto setTemperature(utils::temperature::Temperature newTemp) -> void override;
        virtual auto setTime(std::time_t newTime) -> void override;
        auto setAlarmTriggered() -> void override;
        auto setAlarmActive(bool val) -> void override;
        auto setBottomDescription(const UTF8 &desc) -> void override;
        auto removeBottomDescription() -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setSnoozeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmEdit(bool val) -> void override;
        auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;
        auto setSnoozeTime(std::time_t newTime) -> void override;

        auto getSnoozeTimer() -> SnoozeTimer * override;
        auto getLayout() -> Item * override;

      protected:
        auto setHeaderViewMode(HeaderViewMode mode) -> void;
        virtual auto buildInterface() -> void;
        virtual bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext);

        TimeSetFmtSpinner *time{};
        DuoHBox *statusBox{};
        BellBattery *battery{};
        TextFixedSize *bottomText{};
        AlarmSetSpinner *alarm{};
        SnoozeTimer *snoozeTimer{};
    };
}; // namespace gui
