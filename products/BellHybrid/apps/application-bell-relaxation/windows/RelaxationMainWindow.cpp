// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindow.hpp"
#include <data/RelaxationAudioData.hpp>
#include <data/RelaxationErrorData.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <common/options/OptionBellMenu.hpp>
#include <i18n/i18n.hpp>

namespace
{
    constexpr auto maxDisplayedTitleLength = 30U;
}

namespace gui
{
    RelaxationMainWindow::RelaxationMainWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::relaxation::RelaxationMainWindowContract::Presenter> &&presenter)
        : BellOptionWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
        setListTitle(utils::translate("app_bellmain_relaxation"));
    }

    void RelaxationMainWindow::setSoundsList(std::vector<db::multimedia_files::MultimediaFilesRecord> sounds)
    {
        std::list<gui::Option> menuOptionList;
        auto addRecord = [&](const db::multimedia_files::MultimediaFilesRecord &sound) {
            menuOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                sound.tags.title.substr(0, maxDisplayedTitleLength),
                [=](gui::Item &item) {
                    onActivated(sound);
                    return true;
                },
                [=](gui::Item &item) { return true; },
                this));
        };
        for (const auto &sound : sounds) {
            addRecord(sound);
        }
        addOptions(std::move(menuOptionList));
    }

    void RelaxationMainWindow::buildInterface()
    {
        BellOptionWindow::buildInterface();
        presenter->loadAudioRecords();
    }

    void RelaxationMainWindow::onActivated(const db::multimedia_files::MultimediaFilesRecord &selectedSound)
    {
        auto audioContext = std::make_unique<RelaxationAudioContext>(selectedSound);
        auto switchData   = std::make_unique<RelaxationSwitchData>(std::move(audioContext));
        application->switchWindow(gui::window::name::relaxationTimerSelect, std::move(switchData));
    }

    void RelaxationMainWindow::handleError()
    {
        auto switchData = std::make_unique<RelaxationErrorData>(RelaxationErrorType::FilesLimitExceeded);
        application->switchWindow(gui::window::name::relaxationError, std::move(switchData));
    }

    void RelaxationMainWindow::rebuild()
    {
        presenter->loadAudioRecords();
    }

} // namespace gui
