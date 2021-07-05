// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <date/date.h>

namespace gui
{
    class DateWidget : public VBox
    {
      public:
        DateWidget(Item *parent);
        void loadData(const date::year_month_day &yearMonthDay);

        const date::year_month_day getChosenDate();

      private:
        VBox *vBox        = nullptr;
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
        date::year_month_day validateDate();
        void setDate(int keyValue, Label &item);
        void setOnInputCallback(Label &dateInput);
        void clearInput(Label &dateInput);
    };

} /* namespace gui */
