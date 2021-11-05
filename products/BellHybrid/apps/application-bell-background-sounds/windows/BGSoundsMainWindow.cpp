// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindow.hpp"
#include "data/BGSoundsAudioData.hpp"
#include <application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp>
#include <ApplicationBellBackgroundSounds.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    BGSoundsMainWindow::BGSoundsMainWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> &&presenter)
        : BellOptionWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
        setListTitle(utils::translate("app_bellmain_background_sounds"));
    }
    void BGSoundsMainWindow::setSoundsList(std::vector<db::multimedia_files::MultimediaFilesRecord> sounds)
    {
        std::list<gui::Option> menuOptionList;
        auto addRecord = [&](const db::multimedia_files::MultimediaFilesRecord &sound) {
            menuOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                sound.tags.title,
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
    void BGSoundsMainWindow::buildInterface()
    {
        BellOptionWindow::buildInterface();
        presenter->loadAudioRecords();
    }

    void BGSoundsMainWindow::onActivated(const db::multimedia_files::MultimediaFilesRecord &selectedSound)
    {
        auto audioContext = std::make_unique<BGSoundsAudioContext>(selectedSound);
        auto switchData   = std::make_unique<BGSoundsSwitchData>(std::move(audioContext));
        application->switchWindow(gui::window::name::bgSoundsTimerSelect, std::move(switchData));
    }

} // namespace gui
