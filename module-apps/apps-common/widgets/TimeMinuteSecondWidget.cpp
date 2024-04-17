// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeMinuteSecondWidget.hpp"
#include <common/LanguageUtils.hpp>

namespace
{
    std::vector<std::uint32_t> valueToDigits(std::uint32_t value)
    {
        std::vector<std::uint32_t> digits;
        while (value) {
            digits.push_back(value % 10);
            value /= 10;
        }
        std::reverse(digits.begin(), digits.end());
        return digits;
    }

    namespace digit
    {
        constexpr auto width      = 340U;
        constexpr auto height     = 95U;
        constexpr auto topMargin  = 30U;
        constexpr auto digitWidth = 48U;
        constexpr auto font       = style::window::font::supersizeme;
    } // namespace digit

    namespace description
    {
        constexpr auto width     = 226U;
        constexpr auto height    = 33U;
        constexpr auto topMargin = -2;
        constexpr auto font      = style::window::font::big;
    } // namespace description
} // namespace

namespace gui
{
    TimeMinuteSecondWidget::TimeMinuteSecondWidget(Item *parent,
                                                   const std::uint32_t &x,
                                                   const std::uint32_t &y,
                                                   const std::uint32_t &w,
                                                   const std::uint32_t &h,
                                                   DisplayType type)
        : Rect(parent, x, y, w, h), displayType{type}
    {
        buildInterface(w, h);
    }

    void TimeMinuteSecondWidget::buildInterface(const std::uint32_t w, const std::uint32_t h)
    {
        setEdges(gui::RectangleEdge::None);
        mainBox = new gui::VBox(this, 0, 0, w, h);
        mainBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        mainBox->setEdges(gui::RectangleEdge::None);
        mainBox->activeItem = false;

        digitsContainer = new gui::HBox(mainBox, 0, 0, digit::width, digit::height);
        digitsContainer->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        digitsContainer->setEdges(gui::RectangleEdge::None);
        digitsContainer->setMargins(gui::Margins(0, digit::topMargin, 0, 0));

        for (auto i = 0U; i < maxDigits; i++) {
            digitBox[i] = new gui::VBox(digitsContainer, 0, 0, digit::digitWidth, digit::height);
            digitBox[i]->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            digitBox[i]->setEdges(gui::RectangleEdge::None);
            digitBox[i]->setMargins(gui::Margins(0, 0, 0, 0));

            digitsText[i] = new gui::Label(digitBox[i], 0, 0, 0, 0);
            digitsText[i]->setEdges(gui::RectangleEdge::None);
            digitsText[i]->setMaximumSize(digit::width, digit::height);
            digitsText[i]->setMargins(gui::Margins(0, 0, 0, 0));
            digitsText[i]->setAlignment(
                gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
            digitsText[i]->setFont(digit::font);

            digitBox[i]->resizeItems();
        }
        digitsContainer->resizeItems();

        descriptionBox = new gui::HBox(mainBox, 0, 0, description::width, description::height);
        descriptionBox->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        descriptionBox->setEdges(gui::RectangleEdge::None);
        descriptionBox->setMargins(gui::Margins(0, description::topMargin, 0, 0));

        description = new gui::Label(descriptionBox, 0, 0, 0, 0);
        description->setEdges(gui::RectangleEdge::None);
        description->setMaximumSize(description::width, description::height);
        description->setMargins(gui::Margins(0, 0, 0, 0));
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center});
        description->setFont(description::font);

        descriptionBox->resizeItems();
        mainBox->resizeItems();
    }

    void TimeMinuteSecondWidget::updateTime(std::uint32_t seconds)
    {
        if (seconds >= utils::time::secondsInMinute || displayType == DisplayType::OnlyMinutes) {
            const auto minutes = (seconds + utils::time::secondsInMinute - 1) / utils::time::secondsInMinute;
            setText(minutes);
            description->setText(utils::language::getCorrectMinutesNumeralForm(minutes));
        }
        else {
            setText(seconds);
            description->setText(utils::language::getCorrectSecondsNumeralForm(seconds));
        }
    }

    void TimeMinuteSecondWidget::setText(std::uint32_t value)
    {
        const auto digits      = valueToDigits(value);
        const auto totalDigits = digits.size();

        for (auto i = 0U; i < maxDigits; i++) {
            if (i < totalDigits) {
                digitsText[i]->setText(std::to_string(digits[i]));
                digitBox[i]->setVisible(true);
            }
            else {
                digitBox[i]->setVisible(false);
            }
        }
        digitsContainer->resizeItems();
    }
} /* namespace gui */
