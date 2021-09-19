// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsMainWindow.hpp"
#include "data/BGSoundsAudioData.hpp"
#include <application-bell-background-sounds/ApplicationBellBackgroundSounds.hpp>
#include <ApplicationBellBackgroundSounds.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <i18n/i18n.hpp>

namespace
{
    inline constexpr auto songNamePlaceholder = "Meditative Surprise";
    inline constexpr auto placeHoldersCount   = 6;
} // namespace
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
    void BGSoundsMainWindow::setSoundsList()
    {
        std::list<gui::Option> menuOptionList;
        auto addRecord = [&](const UTF8 &name) {
            menuOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name,
                [=](gui::Item &item) {
                    onActivated(name);
                    return true;
                },
                [=](gui::Item &item) { return true; },
                this));
        };
        for (int i = 0; i < placeHoldersCount; i++)
            addRecord(songNamePlaceholder);

        addOptions(std::move(menuOptionList));
    }
    void BGSoundsMainWindow::buildInterface()
    {
        BellOptionWindow::buildInterface();
        presenter->loadAudioRecords();
    }

    void BGSoundsMainWindow::onActivated(std::string recordName)
    {
        auto audioContext = std::make_unique<BGSoundsAudioContext>(std::move(recordName));
        auto switchData   = std::make_unique<BGSoundsSwitchData>(std::move(audioContext));
        application->switchWindow(gui::window::name::bgSoundsTimerSelect, std::move(switchData));
    }

} // namespace gui
