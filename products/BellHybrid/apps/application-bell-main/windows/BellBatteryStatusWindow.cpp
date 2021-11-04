// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBatteryStatusWindow.hpp"
#include "data/BatteryUtils.hpp"

#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/input/InputEvent.hpp>
#include <Application.hpp>

#include <chrono>

namespace
{
    constexpr auto imageType = gui::ImageTypeSpecifier::W_G;
    constexpr auto batteryEntries =
        std::array<battery_utils::BatteryLevelEntry, 6>{{{{5, 10}, {1, 5}, "bell_status_battery_lvl0"},
                                                         {{11, 30}, {6, 29}, "bell_status_battery_lvl1"},
                                                         {{31, 50}, {30, 53}, "bell_status_battery_lvl2"},
                                                         {{51, 70}, {54, 77}, "bell_status_battery_lvl3"},
                                                         {{71, 95}, {78, 99}, "bell_status_battery_lvl4"},
                                                         {{96, 100}, {100, 100}, "bell_status_battery_lvl5"}}};
} // namespace

namespace gui
{
    using namespace std::chrono_literals;

    BellBatteryStatusWindow::BellBatteryStatusWindow(app::ApplicationCommon *app) : WindowWithTimer{app, name, 5s}
    {
        buildInterface();
    }
    void BellBatteryStatusWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        topDescription = new TextFixedSize(body->firstBox);
        topDescription->setMinimumSize(style::bell_base_layout::outer_layouts_w,
                                       style::bell_base_layout::outer_layouts_h);
        topDescription->setFont(top_description_font);
        topDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        topDescription->setEdges(RectangleEdge::None);
        topDescription->activeItem = false;
        topDescription->drawUnderline(false);
        topDescription->setText(utils::translate("app_settings_tech_info_battery"));

        hbox = new HBox(body->lastBox);
        hbox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        hbox->setEdges(RectangleEdge::None);
        hbox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        chargingIcon = new ImageBox(hbox, 0, 0, 0, 0, new Image("bell_lightning", imageType));
        chargingIcon->setMargins(gui::Margins(0, 0, 8, 0));
        chargingIcon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        chargingIcon->setMinimumSizeToFitImage();
        chargingIcon->activeItem = false;

        bottomDescription = new TextFixedSize(hbox);
        bottomDescription->setMaximumSize(bottom_description_max_size_w, bottom_description_max_size_h);
        bottomDescription->setFont(bottom_description_font);
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->drawUnderline(false);

        center = new ImageBox(body->centerBox,
                              0,
                              0,
                              style::bell_base_layout::w,
                              style::bell_base_layout::h,
                              new Image("bell_status_battery_lvl0", imageType));
        center->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        center->setEdges(RectangleEdge::None);
        center->setMinimumSizeToFitImage();
        center->activeItem = false;

        body->resize();
    }
    bool BellBatteryStatusWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
            return true;
        }
        return false;
    }
    void BellBatteryStatusWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr) {
            WindowWithTimer::onBeforeShow(mode, data);
            const auto &switchData = static_cast<Data &>(*data);
            const auto entry       = battery_utils::getScaledBatteryLevel(batteryEntries, switchData.chargeLevel);
            if (entry) {
                center->setImage(entry->image, imageType);
                bottomDescription->setText(std::to_string(entry->level) + "%");
                chargingIcon->setVisible(switchData.isCharging);
                hbox->resizeItems();
                body->resize();
            }
        }
        else {
            LOG_ERROR("BellBatteryStatusWindow must be invoked with a valid SwitchData");
        }
    }
} // namespace gui
