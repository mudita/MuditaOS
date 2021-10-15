// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        bottomBar->setVisible(false);

        listView = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        listView->setEdges(RectangleEdge::None);

        listView->rebuildList(listview::RebuildType::Full);

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
            presenter->exitWithoutSave();
        }

        return AppWindow::onInput(inputEvent);
    }
    void BellSettingsFrontlightWindow::exit()
    {
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("big_check_W_G",
                                                    utils::translate("app_bell_settings_frontlight_finished_message"),
                                                    window::name::bellSettingsAdvanced));
    }
    void BellSettingsFrontlightWindow::onClose(Window::CloseReason)
    {
        presenter->eraseProviderData();
    }
} /* namespace gui */
