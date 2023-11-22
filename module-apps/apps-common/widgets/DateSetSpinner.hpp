// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        enum class Type
        {
            year,
            month,
            day
        };

        DateSetSpinner(Item *parent, Type type, TextFixedSize *titleBox, Length x, Length y, Length w, Length h);

        date::year_month_day getDate() const;
        void setDate(date::year_month_day date);

      private:
        void applySizeRestrictions();
        std::uint32_t getWidgetMinimumAreaWidth() const;
        std::uint16_t getFontHeight(const std::string &fontName) const;

        template <typename Spinner>
        void attachDateField(Spinner *&field, typename Spinner::range &&range);
        void updateDate();
        bool onInput(const InputEvent &inputEvent) override;

        Type type;
        date::year_month_day date;
        TextFixedSize *title{nullptr};
        U16IntegerSpinnerFixed *year{nullptr};
        U8IntegerSpinnerFixed *dayMonth{nullptr};
    };
} /* namespace gui */
