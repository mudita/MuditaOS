// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateSetSpinner.hpp"

#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <gui/widgets/text/Label.hpp>
#include <time/time_locale.hpp>

namespace gui
{
    namespace
    {
        constexpr std::uint8_t step    = 1;
        constexpr std::uint8_t dayMin  = 1;
        constexpr std::uint8_t dayMax  = 31;
        constexpr std::uint8_t monthMin = 1;
        constexpr std::uint8_t monthMax = 12;
        constexpr auto fontName         = style::window::font::supersizeme;

        void setFont(TextFixedSize *elem, const std::string &fontName)
        {
            elem->setFont(fontName);
            elem->setMinimumHeightToFitText();
            elem->setMinimumWidthToFitText();
            elem->setText(elem->getText());
        }
    } // namespace

    DateSetSpinner::DateSetSpinner(
        Item *parent, Type type, TextFixedSize *title, Length x, Length y, Length w, Length h)
        : HBox(parent, x, y, w, h), type{type}, title{title}
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        switch (type) {
        case Type::year:
            attachDateField(
                year,
                U16IntegerSpinnerFixed::range{utils::time::Locale::min_years, utils::time::Locale::max_years, step});
            year->onValueChanged = [this](auto) { updateDate(); };
            setFocusItem(year);
            break;
        case Type::month:
            attachDateField(dayMonth, U8IntegerSpinnerFixed::range{monthMin, monthMax, step});
            dayMonth->onValueChanged = [this](auto) { updateDate(); };
            setFocusItem(dayMonth);
            break;
        case Type::day:
            attachDateField(dayMonth, U8IntegerSpinnerFixed::range{dayMin, dayMax, step});
            dayMonth->onValueChanged = [this](auto) { updateDate(); };
            setFocusItem(dayMonth);
            break;
        }

        resizeItems();
        applySizeRestrictions();
    }

    date::year_month_day DateSetSpinner::getDate() const
    {
        return date;
    }

    void DateSetSpinner::setDate(const date::year_month_day date)
    {
        this->date = date;
        switch (type) {
        case Type::year:
            year->set_value(static_cast<int>(date.year()));
            break;
        case Type::month:
            dayMonth->set_value(static_cast<unsigned>(date.month()));
            break;
        case Type::day:
            const auto lastDayInMonth =
                static_cast<unsigned>((date::year(date.year()) / date::month(date.month()) / date::last).day());
            const auto day = static_cast<unsigned>(date.day());
            dayMonth->set_range({dayMin, static_cast<std::uint8_t>(lastDayInMonth), step});
            dayMonth->set_value(std::min(day, lastDayInMonth));
            break;
        }
        applySizeRestrictions();
    }

    void DateSetSpinner::updateDate()
    {
        switch (type) {
        case Type::year:
            date = date::year(year->get_value()) / date::month(date.month()) / date::day(date.day());
            break;
        case Type::month:
            date = date::year(date.year()) / date::month(dayMonth->get_value()) / date::day(date.day());
            break;
        case Type::day:
            date = date::year(date.year()) / date::month(date.month()) / date::day(dayMonth->get_value());
            break;
        }
    }

    void DateSetSpinner::applySizeRestrictions()
    {
        if (year != nullptr) {
            year->setMinimumWidthToFitText();
        }
        if (dayMonth != nullptr) {
            dayMonth->setMinimumWidthToFitText();
        }

        setMinimumSize(getWidgetMinimumAreaWidth(), getFontHeight(fontName));
        setMaximumWidth(widgetMaximumArea.w);

        HBox::informContentChanged();
    }

    std::uint32_t DateSetSpinner::getWidgetMinimumAreaWidth() const
    {
        return (type == Type::year) ? year->widgetMinimumArea.w : dayMonth->widgetMinimumArea.w;
    }

    std::uint16_t DateSetSpinner::getFontHeight(const std::string &fontName) const
    {
        const auto font = FontManager::getInstance().getFont(fontName);
        return font->info.line_height;
    }

    template <typename Spinner>
    void DateSetSpinner::attachDateField(Spinner *&field, typename Spinner::range &&range)
    {
        field = new Spinner(std::move(range), Boundaries::Continuous);
        setFont(field, fontName);
        field->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        field->setEdges(RectangleEdge::None);
        field->setPenFocusWidth(0);
        field->set_value(0);
        addWidget(field);
    }

    bool DateSetSpinner::onInput(const InputEvent &inputEvent)
    {
        if (auto ret = this->focusItem->onInput(inputEvent)) {
            applySizeRestrictions();
            return ret;
        }
        return false;
    }
} /* namespace gui */
