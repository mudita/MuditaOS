// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/spinners/Spinners.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/text/TextConstants.hpp>
#include <time/dateCommon.hpp>

#include <string>

namespace gui
{
    class DateSetSpinner : public HBox
    {
      public:
        DateSetSpinner(Item *parent, TextFixedSize *titleBox, Length x, Length y, Length w, Length h);

        date::year_month_day getDate();
        void setDate(date::year_month_day date);

      private:
        void applySizeRestrictions();
        std::uint32_t getWidgetMinimumAreaWidth();
        template <typename Spinner>
        void attachDateField(Spinner *&field, typename Spinner::range &&range);
        void attachSlash(gui::Label *&slash);
        std::uint16_t getFontHeight(const std::string &fontName) const;
        void updateFocus(Item *newFocus);
        bool onInput(const InputEvent &inputEvent) override;
        void clampDayOfMonth();
        bool handleEnterKey();
        bool handleRightFunctionKey();

        TextFixedSize *title         = nullptr;
        U8IntegerSpinnerFixed *day   = nullptr;
        Label *firstSlash            = nullptr;
        U8IntegerSpinnerFixed *month = nullptr;
        Label *secondSlash           = nullptr;
        U16IntegerSpinnerFixed *year = nullptr;

        Item *lastFocus = nullptr;
    };
} /* namespace gui */
