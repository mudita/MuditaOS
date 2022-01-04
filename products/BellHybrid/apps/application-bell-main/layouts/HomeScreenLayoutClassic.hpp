#pragma once

#include "layouts/BaseHomeScreenLayout.hpp"

#include "widgets/BellBattery.hpp"
#include "widgets/StatusBox.hpp"

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;
    class SnoozeTimer;

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
        SnoozeCountdown
    };

    class HomeScreenLayoutClassic : public BaseHomeScreenLayout
    {
      public:
        HomeScreenLayoutClassic(std::string name);

        virtual auto setViewState(app::home_screen::ViewState state) -> void override;
        auto setAlarmTriggered() -> void override;
        auto setAlarmActive(bool val) -> void override;
        auto setTemperature(utils::temperature::Temperature newTemp) -> void override;
        auto setBottomDescription(const UTF8 &desc) -> void override;
        auto removeBottomDescription() -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto setTime(std::time_t newTime) -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setSnoozeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmEdit(bool val) -> void override;
        auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;
        auto setSnoozeTime(std::time_t newTime) -> void override;

        auto getSnoozeTimer() -> SnoozeTimer * override;

      protected:
        auto setHeaderViewMode(HeaderViewMode mode) -> void;
        virtual auto buildInterface() -> void;
        virtual bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext);

        BellBaseLayout *body{};

        TimeSetFmtSpinner *time{};
        StatusBox *statusBox{};
        BellBattery *battery{};
        TextFixedSize *bottomText{};
        AlarmSetSpinner *alarm{};
        SnoozeTimer *snoozeTimer{};

        Item *primaryBottomItem;
    };
}; // namespace gui
