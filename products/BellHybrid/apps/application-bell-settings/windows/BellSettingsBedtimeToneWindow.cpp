// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BellSettingsBedtimeToneWindow.hpp"
#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsStyle.hpp"

#include <common/windows/BellFinishedWindow.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>
#include <popups/data/AudioErrorParams.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    BellSettingsBedtimeToneWindow::BellSettingsBedtimeToneWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::SettingsPresenter::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsBedtimeToneWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsBedtimeToneWindow::buildInterface()
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

    void BellSettingsBedtimeToneWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sidelistview);
    }

    bool BellSettingsBedtimeToneWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->exitWithoutSave();
        }

        return AppWindow::onInput(inputEvent);
    }

    void BellSettingsBedtimeToneWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", gui::name::window::main_window));
    }

    void BellSettingsBedtimeToneWindow::deepRefresh()
    {
        getApplication()->render(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void BellSettingsBedtimeToneWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            presenter->eraseProviderData();
        }
    }

    void BellSettingsBedtimeToneWindow::handleError()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::UnsupportedMediaType);
        app::manager::Controller::sendAction(application, app::manager::actions::ShowPopup, std::move(switchData));
    }

    void BellSettingsBedtimeToneWindow::handleDeletedFile()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::FileDeleted);
        app::manager::Controller::sendAction(application, app::manager::actions::ShowPopup, std::move(switchData));
    }
} /* namespace gui */
