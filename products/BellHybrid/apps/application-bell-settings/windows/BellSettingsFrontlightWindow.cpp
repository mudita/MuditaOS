// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsStyle.hpp"
#include "BellSettingsFrontlightWindow.hpp"

#include <gui/input/InputEvent.hpp>
#include <apps-common/options/OptionStyle.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <widgets/SideListView.hpp>

namespace gui
{
    BellSettingsFrontlightWindow::BellSettingsFrontlightWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::FrontlightPresenter> &&presenter)
        : AppWindow(app, std::move(name)), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsFrontlightWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsFrontlightWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        listView = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        listView->setEdges(RectangleEdge::None);

        listView->rebuildList(listview::RebuildType::Full);
    }

    void BellSettingsFrontlightWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(listView);
    }

    bool BellSettingsFrontlightWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (listView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->revertConfig();
            presenter->setBacklight();
        }

        return AppWindow::onInput(inputEvent);
    }
    void BellSettingsFrontlightWindow::exit()
    {
        presenter->saveConfig();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", window::name::bellSettings));
    }
    void BellSettingsFrontlightWindow::onClose(Window::CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            if (!presenter->isConfigSaved()) {
                presenter->revertConfig();
            }
            presenter->eraseProviderData();
        }
    }
} /* namespace gui */
