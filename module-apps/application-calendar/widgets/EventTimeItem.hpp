// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CalendarListItem.hpp"
#include "EventDateItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class EventTimeItem : public CalendarListItem
    {
        gui::VBox *vBox                = nullptr;
        gui::HBox *hBox                = nullptr;
        gui::Label *colonLabel         = nullptr;
        gui::Label *descriptionLabel   = nullptr;
        gui::Label *hourInput          = nullptr;
        gui::Label *minuteInput        = nullptr;
        gui::Label *mode12hInput       = nullptr;
        DateTimeType dateTimeType      = DateTimeType::Start;
        bool mode24H                   = false;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();
        void setTime(int keyValue, gui::Label &item);
        void onInputCallback(gui::Label &timeInput);
        void clearInput(gui::Label &timeInput);
        bool isPm(const UTF8 &text);
        void validateHour();
        void setMode12hInput(std::chrono::hours hours);

      public:
        EventTimeItem(const std::string &description,
                      DateTimeType dateTimeType,
                      bool mode24H,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() override = default;

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
