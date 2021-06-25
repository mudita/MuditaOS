// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsTimeUnitsWindow.hpp"
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <application-bell-settings/data/BellSettingsStyle.hpp>
#include <apps-common/options/OptionStyle.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BellSettingsTimeUnitsWindow::BellSettingsTimeUnitsWindow(app::Application *app, std::string name)
        : AppWindow(app, name), timeUnitsModel(new TimeUnitsModel(app))
    {
        buildInterface();
    }

    void BellSettingsTimeUnitsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsTimeUnitsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        timeUnitsModel->createData();
        timeUnitsModel->loadData();

        sidelistview = new SideListView(this,
                                        bellSettingsStyle::timeUnits::marginX,
                                        bellSettingsStyle::timeUnits::marginY,
                                        this->getWidth(),
                                        this->getHeight(),
                                        timeUnitsModel);
        setFocusItem(sidelistview);
    }

    bool BellSettingsTimeUnitsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        else {
            if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
                timeUnitsModel->saveData();
                application->returnToPreviousWindow();
                return true;
            }
        }
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        return false;
    }
} /* namespace gui */
