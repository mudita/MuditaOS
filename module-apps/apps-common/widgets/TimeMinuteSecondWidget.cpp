// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "TimeMinuteSecondWidget.hpp"
#include <LanguageUtils.hpp>

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

    std::uint32_t getRoundedMinutes(std::uint32_t currentSeconds, std::optional<std::uint32_t> totalSeconds)
    {
        if (!totalSeconds.has_value()) {
            return 0;
        }

        constexpr auto halfMinute{utils::time::secondsInMinute / 2};
        const auto totalMin   = totalSeconds.value() / utils::time::secondsInMinute;
        const auto totalSec   = totalSeconds.value() % utils::time::secondsInMinute;
        const auto currentMin = currentSeconds / utils::time::secondsInMinute;

        // only at startup, when the number of seconds is less than 30, we round down
        if (totalMin > 0 && totalMin == currentMin && totalSec < halfMinute) {
            return currentMin;
        }
        // otherwise, we round up
        return currentMin + 1;
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

    namespace refreshTime
    {
        constexpr auto defaultValue{std::chrono::seconds{1}};
        constexpr auto arraySize{3};
        constexpr std::array<std::pair<std::uint32_t, std::chrono::seconds>, arraySize> values = {{
            {10 * utils::time::secondsInMinute, std::chrono::seconds{5}},
            {5 * utils::time::secondsInMinute, std::chrono::seconds{3}},
            {3 * utils::time::secondsInMinute, std::chrono::seconds{2}},
        }};
        constexpr auto maxValue = std::max_element(values.begin(), values.end(), [](const auto &l, const auto &r) {
                                      return l.second < r.second;
                                  }) -> second;

        std::chrono::seconds get(std::uint32_t totalSeconds)
        {
            const auto res = std::find_if(
                values.begin(), values.end(), [totalSeconds](const auto &e) { return e.first <= totalSeconds; });
            if (res != values.end()) {
                return res->second;
            }
            return defaultValue;
        }

    } // namespace refreshTime
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

    void TimeMinuteSecondWidget::updateTime(std::uint32_t currentSeconds)
    {
        secondsLeft = currentSeconds;
        updateTotalSeconds(currentSeconds);
        const auto atOrAboveOneMinute = currentSeconds >= utils::time::secondsInMinute;

        if ((displayType != DisplayType::OnlySeconds) &&
            (atOrAboveOneMinute || (displayType == DisplayType::OnlyMinutes))) {
            const auto minutes = getRoundedMinutes(currentSeconds, totalSeconds);
            setText(minutes);
            description->setText(utils::language::getCorrectMinutesNumeralForm(minutes));
        }
        else {
            setText(currentSeconds);
            description->setText(utils::language::getCorrectSecondsNumeralForm(currentSeconds));
        }
    }

    std::chrono::seconds TimeMinuteSecondWidget::getRefreshTime()
    {
        if (!totalSeconds.has_value() ||
            (displayType != DisplayType::OnlyMinutes &&
             secondsLeft < (utils::time::secondsInMinute + refreshTime::maxValue.count()))) {
            return refreshTime::defaultValue;
        }
        return refreshTime::get(totalSeconds.value());
    }

    void TimeMinuteSecondWidget::setText(std::uint32_t value)
    {
        const auto &digits     = valueToDigits(value);
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

    void TimeMinuteSecondWidget::updateTotalSeconds(std::uint32_t currentSeconds)
    {
        if (currentSeconds == 0) {
            totalSeconds.reset();
            return;
        }
        if (!totalSeconds.has_value()) {
            totalSeconds = currentSeconds;
        }
    }
} /* namespace gui */
