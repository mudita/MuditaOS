// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Application.hpp"
#include "AlarmInternalListItem.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <BoxLayout.hpp>
#include <Audio/decoder/Decoder.hpp>

namespace gui
{
    const std::array<AlarmSnooze, 4> snoozeOptions = {
        AlarmSnooze::FiveMinutes, AlarmSnooze::TenMinutes, AlarmSnooze::FifteenMinutes, AlarmSnooze::ThirtyMinutes};

    class AlarmOptionsItem : public AlarmInternalListItem
    {
        enum class MusicStatus
        {
            Stop,
            Play
        };
        app::Application *application = nullptr;
        gui::VBox *vBox               = nullptr;
        gui::HBox *hBox               = nullptr;
        gui::Label *optionLabel       = nullptr;
        gui::Label *descriptionLabel  = nullptr;
        gui::Image *leftArrow         = nullptr;
        gui::Image *rightArrow        = nullptr;
        AlarmOptionItemName itemName;
        std::vector<std::string> optionsNames;
        std::vector<audio::Tags> songsList;
        MusicStatus musicStatus        = MusicStatus::Stop;
        unsigned int actualVectorIndex = 0;
        uint32_t repeatOptionValue     = 0;
        audio::Token token;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;
        void prepareOptionsNames();
        void applyCallbacks();
        std::vector<audio::Tags> getMusicFilesList();

      public:
        AlarmOptionsItem(app::Application *app,
                         AlarmOptionItemName itemName,
                         std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                         std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        void updateAudioToken(audio::Token audioToken);
    };

} /* namespace gui */
