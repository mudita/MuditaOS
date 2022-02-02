// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"

#include "BellSettingsBedtimeToneSettingsWindow.hpp"
#include "BellSettingsStyle.hpp"
#include <common/BellFinishedWindow.hpp>

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace gui
{
    BellSettingsBedtimeToneSettingsWindow::BellSettingsBedtimeToneSettingsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsBedtimeToneSettingsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsBedtimeToneSettingsWindow::buildInterface()
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

    void BellSettingsBedtimeToneSettingsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sidelistview);
    }

    bool BellSettingsBedtimeToneSettingsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void BellSettingsBedtimeToneSettingsWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(
            BellFinishedWindow::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", app::applicationBellSettingsName));
    }

    void BellSettingsBedtimeToneSettingsWindow::onClose(CloseReason reason)
    {
        // presenter->eraseProvideerData();
    }
} /* namespace gui */
