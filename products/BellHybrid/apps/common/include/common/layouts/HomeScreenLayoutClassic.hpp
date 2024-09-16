// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BaseHomeScreenLayoutProvider.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace style::homescreen_classic
{
    inline constexpr auto status_box_layout_w{350U};
    inline constexpr auto info_box_layout_h{70U};
    inline constexpr auto connection_box_layout_h{44U};
    inline constexpr auto connection_box_top_margin{20U};
    inline constexpr auto connection_box_bottom_margin{-20};
} // namespace style::homescreen_classic

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;
    class SnoozeTimer;
    class BellBattery;
    class BellConnectionStatus;
    class DuoHBox;
    class Icon;

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

    enum class ScreenViewMode
    {
        Main,
        Warning
    };

    class HomeScreenLayoutClassic : public BaseHomeScreenLayoutProvider, protected BellBaseLayout
    {
      public:
        explicit HomeScreenLayoutClassic(std::string name);

        auto setViewState(app::home_screen::ViewState state) -> void override;
        auto setTime(std::time_t newTime) -> void override;
        auto setTextDescription(const UTF8 &desc) -> void override;
        auto setBatteryLevelState(const Store::Battery &batteryContext) -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto setSnoozeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        [[nodiscard]] auto getAlarmTime() const -> std::time_t override;
        auto setAlarmTime(std::time_t newTime) -> void override;
        auto setSnoozeTime(std::time_t newTime) -> void override;
        auto updateUsbStatus(bool isConnected) -> void override;

        auto getSnoozeTimer() -> SnoozeTimer * override;
        auto getLayout() -> Item * override;

      protected:
        void setHeaderViewMode(HeaderViewMode mode);
        void setScreenMode(ScreenViewMode mode);
        virtual void buildInterface();
        virtual bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext);
        void removeTextDescription();

      private:
        void adjustConnectionStatusPosition();

      protected:
        VBox *widgetBox{nullptr};
        HBox *timeHBox{nullptr};
        TimeSetFmtSpinner *time{nullptr};
        HBox *statusBox{nullptr};
        HBox *infoBox{nullptr};
        HBox *connectionBox{nullptr};
        BellBattery *battery{nullptr};
        BellConnectionStatus *connectionStatus{nullptr};
        TextFixedSize *bottomText{nullptr};
        AlarmSetSpinner *alarm{nullptr};
        SnoozeTimer *snoozeTimer{nullptr};
        Icon *lowBatteryWarning{nullptr};

        std::function<void()> onShowMessage;
        std::function<void()> onHideMessage;
    };
} // namespace gui
