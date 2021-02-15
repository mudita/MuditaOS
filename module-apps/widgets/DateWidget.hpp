// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/data/dateCommon.hpp"
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Text.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    class DateWidget : public VBox
    {
        HBox *labelsHBox  = nullptr;
        HBox *dateHBox    = nullptr;
        Label *dayLabel   = nullptr;
        Label *monthLabel = nullptr;
        Label *yearLabel  = nullptr;
        Label *dayInput   = nullptr;
        Label *monthInput = nullptr;
        Label *yearInput  = nullptr;

        void buildInterface();
        void applyItemSpecificProperties(Label *item);
        void applyLabelSpecificProperties(Label *label);
        void applyCallbacks();
        calendar::YearMonthDay validateDate();
        void setDate(int keyValue, Label &item);
        void setOnInputCallback(Label &dateInput);
        void clearInput(Label &dateInput);

      public:
        explicit DateWidget(Item *parent);
        ~DateWidget() override = default;

        const calendar::YearMonthDay getChosenDate();

        std::function<void(std::shared_ptr<EventsRecord> event)> onLoadCallback = nullptr;
    };

} /* namespace gui */
