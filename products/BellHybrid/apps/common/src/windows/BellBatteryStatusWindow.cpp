// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "windows/BellBatteryStatusWindow.hpp"
#include <data/BatteryStatusSwitchData.hpp>
#include <common/data/BatteryUtils.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/input/InputEvent.hpp>
#include <Application.hpp>

#include <chrono>

namespace
{
    using namespace std::chrono_literals;

    constexpr auto topDescriptionFont    = style::window::font::largelight;
    constexpr auto bottomDescriptionFont = style::window::font::verybiglight;

    constexpr auto bottomDescriptionMaxSizeW    = style::bell_base_layout::outer_layouts_w;
    constexpr auto bottomDescriptionMaxSizeH    = 85U;
    constexpr auto bottomDescriptionRightMargin = 8U;
    constexpr auto chargingIconRightMargin      = 12U;

    constexpr auto batteryEntries =
        std::array<battery_utils::BatteryLevelEntry, 6>{{{{0, 9}, "bell_status_battery_lvl0"},
                                                         {{10, 19}, "bell_status_battery_lvl1"},
                                                         {{20, 39}, "bell_status_battery_lvl2"},
                                                         {{40, 69}, "bell_status_battery_lvl3"},
                                                         {{70, 95}, "bell_status_battery_lvl4"},
                                                         {{96, 100}, "bell_status_battery_lvl5"}}};

    constexpr auto windowLongTimeout    = 12s; // Must be longer than the long press time (10s) to turn off the system
    constexpr auto windowDefaultTimeout = 3s;
    constexpr auto windowLowBatteryTimeout = 6s;
} // namespace

namespace gui
{
    BellBatteryStatusWindow::BellBatteryStatusWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer{app, name, windowLongTimeout}
    {
        buildInterface();
    }

    void BellBatteryStatusWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(
            this, 0, 0, style::window_width, style::window_height, BellBaseLayout::LayoutType::WithoutArrows);

        topDescription = new TextFixedSize(body->firstBox);
        topDescription->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                       style::bell_base_layout::outer_layouts_h);
        topDescription->setFont(topDescriptionFont);
        topDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        topDescription->setEdges(RectangleEdge::None);
        topDescription->drawUnderline(false);
        topDescription->activeItem = false;

        hbox = new HBox(body->lastBox);
        hbox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        hbox->setEdges(RectangleEdge::None);
        hbox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        chargingIcon = new ImageBox(hbox, 0, 0, 0, 0, new Image("bell_lightning", gui::ImageTypeSpecifier::W_G));
        chargingIcon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        chargingIcon->setMinimumSizeToFitImage();
        chargingIcon->setMargins(gui::Margins(0, 0, chargingIconRightMargin, 0));
        chargingIcon->activeItem = false;

        batteryImage = new ImageBox(body->centerBox,
                                    0,
                                    0,
                                    style::bell_base_layout::w,
                                    style::bell_base_layout::h,
                                    new Image("bell_status_battery_lvl0", gui::ImageTypeSpecifier::W_G));
        batteryImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        batteryImage->setEdges(RectangleEdge::None);
        batteryImage->setMinimumSizeToFitImage();
        batteryImage->activeItem = false;

        bottomDescription = new Text(hbox);
        bottomDescription->setMaximumSize(bottomDescriptionMaxSizeW, bottomDescriptionMaxSizeH);
        bottomDescription->setFont(bottomDescriptionFont);
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->setMargins(gui::Margins(0, 0, bottomDescriptionRightMargin, 0));
        bottomDescription->drawUnderline(false);
        bottomDescription->activeItem = false;

        body->resize();
    }

    bool BellBatteryStatusWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.getKeyCode() == KeyCode::KEY_RF) {
            if (inputEvent.isLongRelease()) {
                // here the "back" button is held all the time, so we set the screen timeout to a long time
                // to stay on this screen until the system shutdown popup is displayed
                resetTimer(windowLongTimeout);
            }
            else {
                // here the "back" button is released, so we set the screen timeout to the default time
                resetTimer(windowDefaultTimeout);
            }
            return true;
        }

        return false;
    }

    void BellBatteryStatusWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            const auto batteryData = dynamic_cast<BatteryStatusSwitchData *>(data);
            if (batteryData == nullptr) {
                LOG_ERROR("Received data nullptr");
                return;
            }
            const auto soc                 = batteryData->getBatteryLevel();
            const bool isCharging          = batteryData->isCharging();
            if (batteryData->isLowBatteryWarning()) {
                topDescription->setText(utils::translate("battery_low"));
                auto tokenMap = text::RichTextParser::TokenMap({{"$BATTERY", std::to_string(soc)}});
                bottomDescription->setRichText(utils::translate("battery_remaining"), std::move(tokenMap));
                chargingIcon->setVisible(false);
                resetTimer(windowLowBatteryTimeout);
            }
            else {
                topDescription->setText(utils::translate("app_settings_tech_info_battery"));
                bottomDescription->setText(std::to_string(soc) + "%");
                chargingIcon->setVisible(isCharging);
            }

            const auto &image = battery_utils::getBatteryLevelImage(batteryEntries, soc);
            if (image.has_value()) {
                batteryImage->setImage(std::string{image.value()}, gui::ImageTypeSpecifier::W_G);
                hbox->resizeItems();
                body->resize();
            }
        }
        WindowWithTimer::onBeforeShow(mode, data);
    }

    void BellBatteryStatusWindow::onClose(CloseReason reason)
    {
        application->popCurrentWindow();
    }
} // namespace gui
