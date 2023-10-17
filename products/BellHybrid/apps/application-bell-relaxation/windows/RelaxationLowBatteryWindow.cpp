// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationLowBatteryWindow.hpp"
#include <data/RelaxationSwitchData.hpp>
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace
{
    gui::TextFixedSize *createBatteryLevelText(gui::VBox *parent)
    {
        auto text = new gui::TextFixedSize(parent);
        text->setFont(gui::relaxationStyle::descriptionFont);
        text->setMinimumWidth(gui::relaxationStyle::text::minWidth);
        text->setMinimumHeightToFitText(gui::relaxationStyle::text::maxLines);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        text->drawUnderline(false);
        return text;
    }
} // namespace

namespace gui
{
    RelaxationLowBatteryWindow::RelaxationLowBatteryWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::relaxation::RelaxationLowBatteryContract::Presenter> &&presenter)
        : WindowWithTimer(app, window::name::relaxationLowBattery), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void RelaxationLowBatteryWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void RelaxationLowBatteryWindow::buildLayout()
    {
        statusBar->setVisible(false);
        auto body = new BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        new gui::Image(body->centerBox, "bell_status_battery_lvl0_W_G");
        batteryText = createBatteryLevelText(body->lastBox);
        body->lastBox->resizeItems();
        body->resizeItems();
    }

    bool RelaxationLowBatteryWindow::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received nullptr");
            return false;
        }

        const auto batteryData = dynamic_cast<RelaxationLowBatterySwitchData *>(data);
        if (batteryData == nullptr) {
            LOG_ERROR("Received data nullptr");
            return false;
        }

        const auto soc = batteryData->getBatteryLevel();
        batteryText->setText(std::to_string(soc) + "%");
        return true;
    }

    void RelaxationLowBatteryWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }
} // namespace gui
