// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsAlarmSettingsSnoozeWindow.hpp"

#include "BellSettingsAlarmSettingsWindow.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <common/BellFinishedWindow.hpp>
#include <gui/input/InputEvent.hpp>
#include <widgets/SideListView.hpp>

namespace gui
{

    BellSettingsAlarmSettingsSnoozeWindow::BellSettingsAlarmSettingsSnoozeWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::SnoozePresenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsAlarmSettingsSnoozeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        listView = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        listView->setEdges(RectangleEdge::None);

        listView->rebuildList(listview::RebuildType::Full);

        presenter->loadData();

        setFocusItem(listView);
    }

    bool BellSettingsAlarmSettingsSnoozeWindow::onInput(const InputEvent &inputEvent)
    {
        if (listView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellSettingsAlarmSettingsSnoozeWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsAlarmSettingsSnoozeWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(BellFinishedWindow::defaultName,
                                  BellFinishedWindowData::Factory::create(
                                      "big_check",
                                      utils::translate("app_bell_settings_alarm_settings_snooze_finished"),
                                      BellSettingsAlarmSettingsWindow::name));
    }
} // namespace gui
