// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AutolockWindow.hpp"

#include <CheckBoxWithLabel.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>
#include <BoxLayout.hpp>

namespace gui
{

    AutolockWindow::AutolockWindow(app::Application *app) : BaseSettingsWindow(app, window::name::autolock)
    {
        buildInterface();
    }

    void AutolockWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_locked_screen_autolock"));

        auto body =
            new VBox(nullptr,
                     0,
                     title->offset_h() + style::margins::big,
                     this->getWidth(),
                     this->getHeight() - style::margins::big - this->title->offset_h() - bottomBar->getHeight());
        body->setEdges(gui::RectangleEdge::None);

        auto toggleBoxes = [this](CheckBoxWithLabel &active) {
            for (CheckBoxWithLabel *box : boxes) {
                box->setChecked(false);
                if (&active == box) {
                    box->setChecked(!box->isChecked());
                    // setAutoLockTime
                }
            }
        };

        std::vector<std::string> autoLockTimes = {"15s", "30s", "1m", "2m", "5m", "10m", "20m"};

        for (auto time : autoLockTimes) {
            boxes.push_back(new CheckBoxWithLabel(body, 0, 0, time, toggleBoxes));
        }

        addWidget(body);
        setFocusItem(body);
    } // namespace gui

} // namespace gui
