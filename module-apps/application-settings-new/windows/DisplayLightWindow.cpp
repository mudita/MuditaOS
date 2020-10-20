// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DisplayLightWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>
#include <Common.hpp>
#include <InputEvent.hpp>

namespace gui
{
    DisplayLightWindow::DisplayLightWindow(app::Application *app) : BaseSettingsWindow(app, window::name::display_light)
    {
        buildInterface();
    }

    void DisplayLightWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_display_light"));

        int32_t offset_h = 8;

        body = new VBox(this,
                        0,
                        title->offset_h() + offset_h,
                        this->getWidth(),
                        this->getHeight() - offset_h - this->title->offset_h() - bottomBar->getHeight());
        body->setEdges(gui::RectangleEdge::None);

        displayLight = new Toggle(nullptr,
                                  0,
                                  0,
                                  body->getWidth(),
                                  style::window::label::big_h,
                                  utils::localize.get("app_settings_display_light_main"),
                                  false);

        autoLight = new Toggle(nullptr,
                               0,
                               0,
                               body->getWidth(),
                               style::window::label::big_h,
                               utils::localize.get("app_settings_display_light_auto"),
                               true);

        brightness = new UpDown(nullptr,
                                0,
                                0,
                                body->getWidth(),
                                style::window::label::big_h,
                                utils::localize.get("app_settings_display_light_brightness"),
                                default_brightness,
                                max_brightness);

        displayLight->activatedCallback = [this](Item &) {
            displayLightSwitchHandler();
            return true;
        };

        autoLight->activatedCallback = [this](Item &) {
            autoLightSwitchHandler();
            return true;
        };

        body->addWidget(displayLight);

        setFocusItem(body);

    } // namespace gui

    void DisplayLightWindow::displayLightSwitchHandler()
    {
        body->removeWidget(autoLight);

        if (displayLight->getState()) {
            body->addWidget(autoLight);
            setFocusItem(body);
            setFocusItem(autoLight);
        }
    }

    void DisplayLightWindow::autoLightSwitchHandler()
    {
        body->removeWidget(brightness);
        if (!autoLight->getState()) {
            body->addWidget(brightness);
            setFocusItem(body);
            setFocusItem(brightness);
        }
    }

    void DisplayLightWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    void DisplayLightWindow::invalidate() noexcept
    {
        displayLight = nullptr;
        autoLight    = nullptr;
        brightness   = nullptr;
    }
} // namespace gui
