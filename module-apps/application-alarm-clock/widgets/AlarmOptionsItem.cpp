// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOptionsItem.hpp"
#include "AlarmClockStyle.hpp"
#include <InputEvent.hpp>
#include <Style.hpp>
#include <Utils.hpp>
#include <module-services/service-audio/service-audio/AudioServiceAPI.hpp>
#include <purefs/filesystem_paths.hpp>

namespace gui
{

    AlarmOptionsItem::AlarmOptionsItem(app::Application *app,
                                       AlarmOptionItemName itemName,
                                       std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                       std::function<void()> bottomBarRestoreFromTemporaryMode)
        : itemName(itemName), bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        application = app;
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, style::alarmClock::window::item::options::height);

        setEdges(RectangleEdge::Bottom);
        setPenWidth(style::window::default_border_rect_no_focus);
        setMargins(gui::Margins(style::margins::small, style::margins::huge / 2, 0, style::margins::huge / 2));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->activeItem = false;

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width,
                                         style::alarmClock::window::item::options::label_h);
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width,
                             style::alarmClock::window::item::options::height -
                                 style::alarmClock::window::item::options::label_h);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        leftArrow = new gui::Image(hBox, 0, 0, 0, 0);
        leftArrow->setMinimumSize(style::alarmClock::window::item::options::arrow_w_h,
                                  style::alarmClock::window::item::options::arrow_w_h);
        leftArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        leftArrow->activeItem = false;
        leftArrow->set("arrow_left");
        leftArrow->setVisible(false);

        optionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        optionLabel->setMinimumSize(
            style::window::default_body_width - 2 * style::alarmClock::window::item::options::arrow_w_h,
            style::alarmClock::window::item::options::height - style::alarmClock::window::item::options::label_h);
        optionLabel->setMargins(gui::Margins(style::alarmClock::window::item::options::arrow_w_h / 2, 0, 0, 0));
        optionLabel->setEdges(gui::RectangleEdge::None);
        optionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionLabel->setFont(style::window::font::medium);
        optionLabel->activeItem = false;

        rightArrow = new gui::Image(hBox, 0, 0, 0, 0);
        rightArrow->setMinimumSize(style::alarmClock::window::item::options::arrow_w_h,
                                   style::alarmClock::window::item::options::arrow_w_h);
        rightArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        rightArrow->activeItem = false;
        rightArrow->set("arrow_right");
        rightArrow->setVisible(false);

        prepareOptionsNames();
        applyCallbacks();
    }

    void AlarmOptionsItem::prepareOptionsNames()
    {
        songsList = getMusicFilesList();
        optionsNames.clear();
        switch (itemName) {
        case AlarmOptionItemName::Sound:
            descriptionLabel->setText(utils::localize.get("app_alarm_clock_sound"));
            break;
        case AlarmOptionItemName::Snooze:
            descriptionLabel->setText(utils::localize.get("app_alarm_clock_snooze"));
            break;
        case AlarmOptionItemName::Repeat:
            descriptionLabel->setText(utils::localize.get("app_alarm_clock_repeat"));
            break;
        }
        if (itemName == AlarmOptionItemName::Sound) {
            for (const auto &musicFile : songsList) {
                optionsNames.push_back(musicFile.title);
            }
        }
        else if (itemName == AlarmOptionItemName::Snooze) {
            optionsNames.push_back(utils::localize.get("app_alarm_clock_snooze_5_min"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_snooze_10_min"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_snooze_15_min"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_snooze_30_min"));
        }
        else if (itemName == AlarmOptionItemName::Repeat) {
            optionsNames.push_back(utils::localize.get("app_alarm_clock_repeat_never"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_repeat_everyday"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_repeat_week_days"));
            optionsNames.push_back(utils::localize.get("app_alarm_clock_repeat_custom"));
        }
    }

    void AlarmOptionsItem::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (item.focus) {
                optionLabel->setFont(style::window::font::mediumbold);
                optionLabel->setMargins(gui::Margins(0, 0, 0, 0));
                leftArrow->setVisible(true);
                rightArrow->setVisible(true);
                hBox->resizeItems();
                if (itemName == AlarmOptionItemName::Sound) {
                    bottomBarTemporaryMode(utils::localize.get("app_alarm_clock_play_pause"));
                }
                if (itemName == AlarmOptionItemName::Repeat && actualVectorIndex == optionsNames.size() - 1) {
                    bottomBarTemporaryMode(utils::localize.get("app_alarm_clock_edit"));
                }
            }
            else {
                optionLabel->setFont(style::window::font::medium);
                optionLabel->setMargins(gui::Margins(style::alarmClock::window::item::options::arrow_w_h / 2, 0, 0, 0));
                leftArrow->setVisible(false);
                rightArrow->setVisible(false);
                hBox->resizeItems();
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_RF)) {
                setFocusItem(nullptr);
            }

            if (event.is(gui::KeyCode::KEY_LEFT)) {
                actualVectorIndex--;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = optionsNames.size() - 1;
                    if (itemName == AlarmOptionItemName::Repeat) {
                        bottomBarTemporaryMode(utils::localize.get("app_alarm_clock_edit"));
                    }
                }
                else if (itemName == AlarmOptionItemName::Repeat) {
                    bottomBarRestoreFromTemporaryMode();
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                return true;
            }

            if (event.is(gui::KeyCode::KEY_RIGHT)) {
                actualVectorIndex++;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = 0;
                }
                if (actualVectorIndex == optionsNames.size() - 1 && itemName == AlarmOptionItemName::Repeat) {
                    bottomBarTemporaryMode(utils::localize.get("app_alarm_clock_edit"));
                }
                else if (itemName == AlarmOptionItemName::Repeat) {
                    bottomBarRestoreFromTemporaryMode();
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                return true;
            }

            if (event.is(gui::KeyCode::KEY_LF) && itemName == AlarmOptionItemName::Repeat &&
                actualVectorIndex == optionsNames.size() - 1) {
                OptionParser parser;
                auto weekDayRepeatData = std::make_unique<WeekDaysRepeatData>();
                auto weekDayData       = parser.setWeekDayOptions(repeatOptionValue, std::move(weekDayRepeatData));
                application->switchWindow(style::alarmClock::window::name::customRepeat, std::move(weekDayData));
            }

            if (event.is(gui::KeyCode::KEY_LF) && itemName == AlarmOptionItemName::Sound) {
                if (musicStatus == MusicStatus::Stop) {
                    musicStatus = MusicStatus::Play;
                    AudioServiceAPI::PlaybackStart(
                        application, audio::PlaybackType::Multimedia, songsList[actualVectorIndex].filePath);
                }
                else if (musicStatus == MusicStatus::Play) {
                    musicStatus = MusicStatus::Stop;
                    AudioServiceAPI::StopAll(application);
                }
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<AlarmsRecord> alarm) {
            switch (itemName) {
            case AlarmOptionItemName::Sound: {
                alarm->path = songsList[actualVectorIndex].filePath;
                break;
            }
            case AlarmOptionItemName::Snooze: {
                alarm->snooze = static_cast<uint32_t>(snoozeOptions[actualVectorIndex]);
                break;
            }
            case AlarmOptionItemName::Repeat: {
                if (alarm->repeat < optionsNames.size() - 1 && actualVectorIndex != optionsNames.size() - 1) {
                    alarm->repeat = actualVectorIndex;
                }
                else if (alarm->repeat == optionsNames.size() - 1 ||
                         optionsNames[optionsNames.size() - 1] ==
                             utils::localize.get("app_alarm_clock_repeat_custom")) {
                    alarm->repeat = static_cast<uint32_t>(AlarmRepeat::never);
                }
                break;
            }
            }
        };

        onLoadCallback = [&](std::shared_ptr<AlarmsRecord> alarm) {
            switch (itemName) {
            case AlarmOptionItemName::Sound: {
                auto it = std::find_if(songsList.begin(), songsList.end(), [alarm](const audio::Tags &tag) {
                    return tag.filePath == alarm->path.c_str();
                });
                if (it == songsList.end()) {
                    LOG_DEBUG("No such song in the list");
                    actualVectorIndex = 0;
                }
                else {
                    actualVectorIndex = std::distance(songsList.begin(), it);
                }
                break;
            }
            case AlarmOptionItemName::Snooze: {
                auto it =
                    std::find(snoozeOptions.begin(), snoozeOptions.end(), static_cast<AlarmSnooze>(alarm->snooze));
                if (it == snoozeOptions.end()) {
                    actualVectorIndex = 0;
                }
                else {
                    actualVectorIndex = std::distance(snoozeOptions.begin(), it);
                }
                break;
            }
            case AlarmOptionItemName::Repeat: {
                if (alarm->repeat < optionsNames.size() - 1) {
                    actualVectorIndex = alarm->repeat;
                    if (alarm->repeat == static_cast<uint32_t>(AlarmRepeat::never)) {
                        optionsNames[optionsNames.size() - 1] = utils::localize.get("app_alarm_clock_repeat_custom");
                    }
                    bottomBarRestoreFromTemporaryMode();
                }
                else {
                    auto parser = CustomRepeatValueParser(alarm->repeat);
                    if (parser.isCustomValueEveryday()) {
                        actualVectorIndex = static_cast<uint32_t>(AlarmRepeat::everyday);
                        alarm->repeat     = actualVectorIndex;
                        bottomBarRestoreFromTemporaryMode();
                        optionsNames[optionsNames.size() - 1] = utils::localize.get("app_alarm_clock_repeat_custom");
                    }
                    else if (parser.isCustomValueWeekDays()) {
                        actualVectorIndex = static_cast<uint32_t>(AlarmRepeat::weekDays);
                        alarm->repeat     = actualVectorIndex;
                        bottomBarRestoreFromTemporaryMode();
                        optionsNames[optionsNames.size() - 1] = utils::localize.get("app_alarm_clock_repeat_custom");
                    }
                    else {
                        actualVectorIndex                     = optionsNames.size() - 1;
                        optionsNames[optionsNames.size() - 1] = parser.getWeekDaysText();
                        if (this->focus) {
                            bottomBarTemporaryMode(utils::localize.get("app_alarm_clock_edit"));
                        }
                    }
                }
                repeatOptionValue = alarm->repeat;
                break;
            }
            }
            optionLabel->setText(optionsNames[actualVectorIndex]);
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    std::vector<audio::Tags> AlarmOptionsItem::getMusicFilesList()
    {
        const auto musicFolder = (purefs::dir::getUserDiskPath() / "music").string();
        std::vector<audio::Tags> musicFiles;
        LOG_INFO("Scanning music folder: %s", musicFolder.c_str());
        for (const auto &ent : std::filesystem::directory_iterator(musicFolder)) {
            if (!ent.is_directory()) {
                const auto filePath = std::string(musicFolder) + "/" + ent.path().filename().c_str();
                auto fileTags       = AudioServiceAPI::GetFileTags(application, filePath);
                if (fileTags) {
                    musicFiles.push_back(*fileTags);
                    LOG_DEBUG("file: %s found", ent.path().filename().c_str());
                }
                else {
                    LOG_ERROR("Not an audio file %s", ent.path().filename().c_str());
                }
            }
        }
        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
        return musicFiles;
    }
} /* namespace gui */
