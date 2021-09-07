// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOptionsItem.hpp"
#include "AlarmClockStyle.hpp"
#include <InputEvent.hpp>
#include <Style.hpp>
#include <Utils.hpp>
#include <service-audio/AudioServiceAPI.hpp>
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

        // create audioOperations to allow shounds preview
        audioOperations = std::make_unique<app::AsyncAudioOperations>(app);

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
            descriptionLabel->setText(utils::translate("app_alarm_clock_sound"));
            break;
        case AlarmOptionItemName::Snooze:
            descriptionLabel->setText(utils::translate("app_alarm_clock_snooze"));
            break;
        case AlarmOptionItemName::Repeat:
            descriptionLabel->setText(utils::translate("app_alarm_clock_repeat"));
            break;
        }
        if (itemName == AlarmOptionItemName::Sound) {
            for (const auto &musicFile : songsList) {
                optionsNames.push_back(musicFile.title);
            }
        }
        else if (itemName == AlarmOptionItemName::Snooze) {
            optionsNames.push_back(utils::translate("app_alarm_clock_snooze_5_min"));
            optionsNames.push_back(utils::translate("app_alarm_clock_snooze_10_min"));
            optionsNames.push_back(utils::translate("app_alarm_clock_snooze_15_min"));
            optionsNames.push_back(utils::translate("app_alarm_clock_snooze_30_min"));
        }
        else if (itemName == AlarmOptionItemName::Repeat) {
            optionsNames.push_back(utils::translate("app_alarm_clock_repeat_never"));
            optionsNames.push_back(utils::translate("app_alarm_clock_repeat_everyday"));
            optionsNames.push_back(utils::translate("app_alarm_clock_repeat_week_days"));
            optionsNames.push_back(utils::translate("app_alarm_clock_repeat_custom"));
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
                    bottomBarTemporaryMode(utils::translate(style::strings::common::play_pause));
                }
                if (itemName == AlarmOptionItemName::Repeat && actualVectorIndex == optionsNames.size() - 1) {
                    bottomBarTemporaryMode(utils::translate("app_alarm_clock_edit"));
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

            // stop preview playback when we loose focus
            if (itemName == AlarmOptionItemName::Sound && musicStatus == MusicStatus::Play) {
                stopAudioPreview();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
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
                        bottomBarTemporaryMode(utils::translate("app_alarm_clock_edit"));
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
                    bottomBarTemporaryMode(utils::translate("app_alarm_clock_edit"));
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
                if (songsList[actualVectorIndex].filePath != currentlyPreviewedPath) {
                    playAudioPreview(songsList[actualVectorIndex].filePath);
                }
                else if (musicStatus == MusicStatus::Stop) {
                    resumeAudioPreview();
                }
                else {
                    pauseAudioPreview();
                }
            }

            // stop preview playback when we go back
            if (itemName == AlarmOptionItemName::Sound && musicStatus == MusicStatus::Play &&
                event.is(gui::KeyCode::KEY_RF)) {
                stopAudioPreview();
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            switch (itemName) {
            case AlarmOptionItemName::Sound: {
                // stop preview playback if it is played
                if (musicStatus == MusicStatus::Play) {
                    stopAudioPreview();
                }
                alarm->musicTone = songsList[actualVectorIndex].filePath;
                break;
            }
            case AlarmOptionItemName::Snooze: {
                alarm->snoozeDuration = static_cast<uint32_t>(snoozeOptions[actualVectorIndex]);
                break;
            }
            case AlarmOptionItemName::Repeat: {
                break;
            }
            }
        };

        onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
            switch (itemName) {
            case AlarmOptionItemName::Sound: {
                auto it = std::find_if(songsList.begin(), songsList.end(), [alarm](const tags::fetcher::Tags &tag) {
                    return tag.filePath == alarm->musicTone.c_str();
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
                auto it = std::find(
                    snoozeOptions.begin(), snoozeOptions.end(), static_cast<AlarmSnooze>(alarm->snoozeDuration));
                if (it == snoozeOptions.end()) {
                    actualVectorIndex = 0;
                }
                else {
                    actualVectorIndex = std::distance(snoozeOptions.begin(), it);
                }
                break;
            }
            case AlarmOptionItemName::Repeat: {
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

    std::vector<tags::fetcher::Tags> AlarmOptionsItem::getMusicFilesList()
    {
        const auto musicFolder = (purefs::dir::getUserDiskPath() / "music").string();
        std::vector<tags::fetcher::Tags> musicFiles;
        LOG_INFO("Scanning music folder: %s", musicFolder.c_str());
        for (const auto &ent : std::filesystem::directory_iterator(musicFolder)) {
            if (!ent.is_directory()) {
                const auto filePath = std::string(musicFolder) + "/" + ent.path().filename().c_str();
                auto fileTags       = tags::fetcher::fetchTags(filePath);
                musicFiles.push_back(fileTags);
                LOG_DEBUG("file: %s found", ent.path().filename().c_str());
            }
        }
        LOG_INFO("Total number of music files found: %u", static_cast<unsigned int>(musicFiles.size()));
        return musicFiles;
    }

    bool AlarmOptionsItem::playAudioPreview(const std::string &path)
    {
        return audioOperations->play(path, [this, path](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus             = MusicStatus::Play;
            currentlyPreviewedToken = token;
            currentlyPreviewedPath  = path;
        });
    }

    bool AlarmOptionsItem::pauseAudioPreview()
    {
        return audioOperations->pause(currentlyPreviewedToken, [this](audio::RetCode retCode, audio::Token token) {
            if (token != currentlyPreviewedToken) {
                LOG_ERROR("Audio preview pause failed: wrong token");
                return;
            }
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus = MusicStatus::Stop;
        });
    }

    bool AlarmOptionsItem::resumeAudioPreview()
    {
        return audioOperations->resume(currentlyPreviewedToken, [this](audio::RetCode retCode, audio::Token token) {
            if (token != currentlyPreviewedToken) {
                LOG_ERROR("Audio preview resume failed: wrong token");
                return;
            }

            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview pause failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            musicStatus = MusicStatus::Play;
        });
    }

    bool AlarmOptionsItem::stopAudioPreview()
    {
        if (currentlyPreviewedToken.IsValid()) {
            musicStatus            = MusicStatus::Stop;
            currentlyPreviewedPath = "";
            return audioOperations->stop(currentlyPreviewedToken, [](audio::RetCode, audio::Token) {});
        }
        return false;
    }
} /* namespace gui */
