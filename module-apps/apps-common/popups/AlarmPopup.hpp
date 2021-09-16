// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <string>

namespace style::popup::alarm
{
    namespace window
    {
        inline constexpr auto y = 76U;
    }

    namespace ImageCircle
    {
        inline constexpr auto name = "vertical_arcs";
    } // namespace ImageCircle

    namespace AlarmTimeLabel
    {
        inline constexpr auto top_margin    = 82U;
        inline constexpr auto bottom_margin = 10U;
    } // namespace AlarmTimeLabel

    namespace SnoozeLabel
    {
        inline constexpr auto h          = 58U;
        inline constexpr auto top_margin = 104U;
    } // namespace SnoozeLabel
} // namespace style::popup::alarm

namespace gui
{
    class AlarmPopup : public AppWindow
    {
      public:
        AlarmPopup(app::ApplicationCommon *app, const std::string &name);
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

      private:
        VBox *body = nullptr;

        void addArcOverlay();
        void createMainLayout();
        void addAlarmLabels(std::string timeString);
        void addSnoozeLabel(std::string timeString);
    };

}; // namespace gui
