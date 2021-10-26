// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ColorTestWindow.hpp"

#include <application-settings/widgets/advanced/ColorTestListStyle.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <service-gui/Common.hpp>
#include <service-gui/messages/ChangeColorScheme.hpp>

namespace gui
{
    static gui::ColorScheme currentColorScheme = gui::Color::defaultColorScheme;

    ColorTestWindow::ColorTestWindow(app::ApplicationCommon *app)
        : AppWindow{app, window::name::color_test_window}, colorTestModel{std::make_shared<ColorTestModel>(app)}
    {
        buildInterface();
    }

    void ColorTestWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_title_color_test"));

        colorListView = new ColorTestListView(application,
                                              this,
                                              style::colorTest::listview::x,
                                              style::colorTest::listview::y,
                                              style::colorTest::listview::width,
                                              style::colorTest::listview::height,
                                              colorTestModel);

        colorListView->rebuildList();

        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setText(nav_bar::Side::Left, utils::translate("app_settings_toolbar_reset"));
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::save));
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        applyInputCallback();

        setFocusItem(colorListView);
    }

    void ColorTestWindow::applyInputCallback()
    {
        this->inputCallback = [&](Item &item, const InputEvent &event) -> bool {
            if (event.isShortRelease()) {
                if (event.is(KeyCode::KEY_ENTER)) {
                    setGlobalColorScheme(colorTestModel->getColorScheme());
                    return true;
                }
                else if (event.is(KeyCode::KEY_LF)) {
                    resetGlobalColorScheme();
                    return true;
                }
            }
            return false;
        };
    }

    void ColorTestWindow::resetGlobalColorScheme()
    {
        colorTestModel->createData();
        colorListView->rebuildList();
        setGlobalColorScheme(gui::Color::defaultColorScheme);
    }

    void ColorTestWindow::setGlobalColorScheme(const ColorScheme &scheme)
    {
        if (scheme != currentColorScheme) {
            currentColorScheme = scheme;
            application->bus.sendUnicastSync(
                std::make_shared<service::gui::ChangeColorScheme>(std::move(scheme)), service::name::gui, 100);
            LOG_INFO("Updated color scheme");

            application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
        }
    }
} /* namespace gui */
