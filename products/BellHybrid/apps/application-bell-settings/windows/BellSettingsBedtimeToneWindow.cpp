// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsBedtimeToneWindow.hpp"
#include "application-bell-settings/ApplicationBellSettings.hpp"

#include <common/options/BellOptionsNavigation.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    BellSettingsBedtimeToneWindow::BellSettingsBedtimeToneWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::BedtimeSettingsWindowContract::Presenter> &&presenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsBedtimeToneWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        songList = new gui::ListViewWithArrows(
            this, 0, 0, style::window_width, style::window_height, presenter->getSongsModel());
        songList->applySizeRestrictions(style::bell_options_list::w,
                                        style::bell_options_list::h,
                                        style::bell_options_list::outer_layouts_h,
                                        style::bell_options_list::outer_layouts_margin);

        songList->setListTitle(utils::translate("app_bellmain_relaxation"));

        auto storedCallback     = songList->inputCallback;
        songList->inputCallback = [&, storedCallback](Item &item, const InputEvent &event) {
            return storedCallback(item, invertNavigationDirection(event));
        };
        setFocusItem(songList);

        auto activateCallback = [this](const db::multimedia_files::MultimediaFilesRecord &selectedSound) {
            // TODO: [CS] save to DataBase
            return true;
        };

        auto scrollCallback = [this](const db::multimedia_files::MultimediaFilesRecord &selectedSound) {
            presenter->playSong(selectedSound.fileInfo.path);
            return true;
        };

        presenter->createData(std::move(activateCallback), std::move(scrollCallback));
    }

    void BellSettingsBedtimeToneWindow::handleError()
    {
        // TODO: [CS] handle error
        // auto switchData = std::make_unique<RelaxationErrorData>(RelaxationErrorType::FilesLimitExceeded);
        // application->switchWindow(gui::window::name::relaxationError, std::move(switchData));
    }

    void BellSettingsBedtimeToneWindow::onClose(CloseReason reason)
    {
        presenter->stopSong();
    }

    void BellSettingsBedtimeToneWindow::updateViewState()
    {
        songList->rebuildList(gui::listview::RebuildType::InPlace);
    }
    void BellSettingsBedtimeToneWindow::rebuild()
    {
        presenter->updateRecordsCount();
        songList->reset();
        songList->rebuildList(gui::listview::RebuildType::Full);
    }
} /* namespace gui */
