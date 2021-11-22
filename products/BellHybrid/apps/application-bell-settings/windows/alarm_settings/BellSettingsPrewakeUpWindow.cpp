// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsAlarmSettingsMenuWindow.hpp"
#include "BellSettingsPrewakeUpWindow.hpp"
#include "BellSettingsStyle.hpp"
#include <common/windows/BellFinishedWindow.hpp>

#include <apps-common/options/OptionStyle.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace gui
{
    BellSettingsPrewakeUpWindow::BellSettingsPrewakeUpWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::PrewakeUpWindowContract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsPrewakeUpWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsPrewakeUpWindow::buildInterface()
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

        setFocusItem(sidelistview);
    }

    bool BellSettingsPrewakeUpWindow::onInput(const gui::InputEvent &inputEvent)
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

    void BellSettingsPrewakeUpWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", BellSettingsAlarmSettingsMenuWindow::name));
    }

    void BellSettingsPrewakeUpWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            presenter->eraseProviderData();
        }
    }
} /* namespace gui */
