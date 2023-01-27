// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/AlarmPresenter.hpp"

#include <AppWindow.hpp>
#include <TextFixedSize.hpp>
#include <ImageBoxWithText.hpp>

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
    class AlarmPopup : public AppWindow, app::popup::AlarmPopupContract::View
    {
      public:
        AlarmPopup(app::ApplicationCommon *app,
                   const std::string &name,
                   std::shared_ptr<app::popup::AlarmPopupPresenter> presenter);
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

      private:
        VBox *body                         = nullptr;
        VBox *snoozeLabelBox               = nullptr;
        gui::TextFixedSize *alarmLabel     = nullptr;
        gui::TextFixedSize *alarmTimeLabel = nullptr;
        gui::TextFixedSize *snoozeLabel    = nullptr;
        gui::ImageBoxWithText *snoozeIcon  = nullptr;

        void addArcOverlay();
        void createMainLayout();
        void createLabels();
        void createSnoozeButton();
        void updateItems();
        void refillText();
    };

}; // namespace gui
