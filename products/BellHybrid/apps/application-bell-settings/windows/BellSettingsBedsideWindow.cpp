// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BellSettingsBedsideWindow.hpp"
#include "application-bell-settings/ApplicationBellSettings.hpp"
#include <common/windows/BellFinishedWindow.hpp>
#include <widgets/SideListView.hpp>

namespace gui
{
    BellSettingsBedsideWindow::BellSettingsBedsideWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::BedsidePresenter> &&presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    auto BellSettingsBedsideWindow::buildInterface() -> void
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        listView =
            new SideListView(this, 0U, 0U, getWidth(), getHeight(), presenter->getPagesProvider(), PageBarType::None);
        listView->setEdges(RectangleEdge::None);

        listView->rebuildList(listview::RebuildType::Full);

        presenter->loadData();
    }

    auto BellSettingsBedsideWindow::onBeforeShow([[maybe_unused]] gui::ShowMode mode,
                                                 [[maybe_unused]] gui::SwitchData *data) -> void
    {
        setFocusItem(listView);
    }

    auto BellSettingsBedsideWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (listView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return false;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->exitWithoutSave();
        }

        return AppWindow::onInput(inputEvent);
    }

    auto BellSettingsBedsideWindow::onClose(CloseReason reason) -> void
    {
        if (reason != CloseReason::Popup) {
            presenter->eraseProviderData();
        }
    }

    auto BellSettingsBedsideWindow::rebuild() -> void
    {
        erase();
        buildInterface();
    }

    auto BellSettingsBedsideWindow::exit() -> void
    {
        presenter->saveData();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", window::name::bellSettings));
    }
} // namespace gui
