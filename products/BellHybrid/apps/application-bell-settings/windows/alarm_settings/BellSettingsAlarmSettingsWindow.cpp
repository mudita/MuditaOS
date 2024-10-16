// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsAlarmSettingsMenuWindow.hpp"
#include "BellSettingsAlarmSettingsWindow.hpp"
#include "BellSettingsStyle.hpp"

#include <common/windows/BellFinishedWindow.hpp>
#include <popups/data/AudioErrorParams.hpp>
#include <service-appmgr/Controller.hpp>
#include <apps-common/options/OptionStyle.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace gui
{
    BellSettingsAlarmSettingsWindow::BellSettingsAlarmSettingsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsAlarmSettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        sidelistview = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sidelistview->setEdges(RectangleEdge::None);

        sidelistview->rebuildList(listview::RebuildType::Full);

        presenter->loadData();
    }

    bool BellSettingsAlarmSettingsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->exitWithRollback();
        }

        return AppWindow::onInput(inputEvent);
    }

    void BellSettingsAlarmSettingsWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", BellSettingsAlarmSettingsMenuWindow::name));
    }

    void BellSettingsAlarmSettingsWindow::deepRefresh()
    {
        getApplication()->render(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void BellSettingsAlarmSettingsWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            presenter->eraseProviderData();
        }
    }

    void BellSettingsAlarmSettingsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sidelistview);
    }

    void BellSettingsAlarmSettingsWindow::handleError()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::UnsupportedMediaType);
        app::manager::Controller::sendAction(application, app::manager::actions::ShowPopup, std::move(switchData));
    }

    void BellSettingsAlarmSettingsWindow::handleDeletedFile()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::FileDeleted);
        app::manager::Controller::sendAction(application, app::manager::actions::ShowPopup, std::move(switchData));
    }
} /* namespace gui */
