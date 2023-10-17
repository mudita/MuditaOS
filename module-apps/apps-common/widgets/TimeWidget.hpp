// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateWidget.hpp"
#include <time/FromTillDate.hpp>

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
                   std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                   std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);
        void loadData(const std::chrono::hours &hoursFrom,
                      const std::chrono::minutes &minutesFrom,
                      const std::chrono::hours &hoursTill,
                      const std::chrono::minutes &minutesTill);
        bool saveData(std::shared_ptr<utils::time::FromTillDate> fromTillDate) const;
        virtual ~TimeWidget() override = default;

        void setConnectionToSecondItem(TimeWidget *item);
        void setConnectionToDateItem(DateWidget *item);

      private:
        VBox *vBox              = nullptr;
        HBox *hBox              = nullptr;
        Label *colonLabel       = nullptr;
        Label *descriptionLabel = nullptr;
        Label *hourInput        = nullptr;
        Label *minuteInput      = nullptr;
        Label *mode12hInput     = nullptr;
        bool mode24H            = false;
        TimeWidget *secondItem  = nullptr;
        DateWidget *dateItem    = nullptr;

        Type type;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();
        void prepareMode12HInputLabel();
        void setTime(int keyValue, Label &item);
        void onInputCallback(Label &timeInput);
        void clearInput(Label &timeInput);
        inline bool isPm(const std::string &text) const;
        bool validateHour() const;
        void validateHourFor12hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes) const;
        void validateHourFor24hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes) const;
    };
} /* namespace gui */
