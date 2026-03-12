// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/layouts/BaseHomeScreenLayoutProvider.hpp>
#include <widgets/BoxLayout.hpp>

namespace gui
{
    class ClockBinary;
    class BellConnectionStatus;
    class AlarmIcon;
    class BellBattery;
    class BellBaseLayout;
    class TimeSetFmtSpinner;
    class Icon;

    class HomeScreenLayoutBinary : public BaseHomeScreenLayoutProvider, public HBox
    {
      public:
        enum class ScreenMode
        {
            Main,
            AlarmEdit,
            AlarmActivatedDeactivated,
        };

        explicit HomeScreenLayoutBinary(std::string name);

        auto setViewState(app::home_screen::ViewState state) -> void override;
        auto setTextDescription(const UTF8 &desc) -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto updateUsbStatus(bool isConnected) -> void override;
        auto setTime(std::time_t newTime) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        [[nodiscard]] auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;
        auto getSnoozeTimer() -> SnoozeTimer * override;
        auto getLayout() -> Item * override;

      private:
        VBox *mainScreen{nullptr};
        HBox *topBox{nullptr};
        HBox *centerBox{nullptr};
        HBox *bottomBox{nullptr};
        ClockBinary *clock{nullptr};
        BellConnectionStatus *connectionStatus{nullptr};
        AlarmIcon *alarmMainIcon{nullptr};
        BellBattery *battery{nullptr};

        BellBaseLayout *setAlarmScreen{nullptr};
        AlarmIcon *alarmTopIcon{nullptr};
        TimeSetFmtSpinner *setAlarmFmtSpinner{nullptr};

        Icon *alarmActivationScreen{nullptr};

        auto setScreenMode(ScreenMode mode) -> void;
        auto setAlarmActivationImage(const UTF8 &name, ImageTypeSpecifier specifier) -> void;
    };
} // namespace gui
