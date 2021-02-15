// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateWidget.hpp"
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    namespace timeConstants
    {
        inline constexpr auto before_noon = "AM";
        inline constexpr auto after_noon  = "PM";
    } // namespace timeConstants

    class TimeWidget : public VBox
    {
      public:
        enum class Type
        {
            Start,
            End
        };

        TimeWidget(Item *parent,
                   const std::string &description,
                   Type type,
                   std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                   std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~TimeWidget() override = default;

        void setConnectionToSecondItem(gui::TimeWidget *item);
        void setConnectionToDateItem(gui::DateWidget *item);

        std::function<void(std::shared_ptr<EventsRecord> event)> onLoadCallback = nullptr;
        std::function<bool(std::shared_ptr<EventsRecord> event)> onSaveCallback = nullptr;

      private:
        gui::HBox *hBox              = nullptr;
        gui::Label *colonLabel       = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Label *hourInput        = nullptr;
        gui::Label *minuteInput      = nullptr;
        gui::Label *mode12hInput     = nullptr;
        bool mode24H                 = false;
        gui::TimeWidget *secondItem  = nullptr;
        gui::DateWidget *dateItem    = nullptr;

        Type type;
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();
        void prepareMode12HInputLabel();
        void setTime(int keyValue, gui::Label &item);
        void onInputCallback(gui::Label &timeInput);
        void clearInput(gui::Label &timeInput);
        bool isPm(const std::string &text);
        bool validateHour();
        void validateHourFor12hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);
        void validateHourFor24hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);
        TimePoint calculateEventTime(calendar::YearMonthDay date,
                                     std::chrono::hours hours,
                                     std::chrono::minutes minutes);
    };
} /* namespace gui */
