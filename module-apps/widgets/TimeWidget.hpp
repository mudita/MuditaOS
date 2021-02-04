// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateOrTimeListItem.hpp"
#include "widgets/DateWidget.hpp"

namespace gui
{
    namespace timeConstants
    {
        inline constexpr auto before_noon = "AM";
        inline constexpr auto after_noon  = "PM";
    } // namespace timeConstants

    class TimeWidget : public DateOrTimeListItem
    {
      public:
        enum class Type
        {
            Start,
            End
        };

        TimeWidget(const std::string &description,
                   Type type,
                   std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                   std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~TimeWidget() override = default;

        void setConnectionToSecondItem(gui::TimeWidget *item);
        void setConnectionToDateItem(gui::DateWidget *item);
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      private:
        gui::VBox *vBox              = nullptr;
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
