// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeFixedWidget.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <time/time_date_validation.hpp>
#include "DateAndTimeStyle.hpp"
#include <time/TimeZone.hpp>
#include <service-time/api/TimeSettingsApi.hpp>

namespace
{
    constexpr auto colonSign = ":";

    std::uint32_t getWidestDigitWidth(const gui::RawFont *const font)
    {
        std::uint32_t widestWidth = 0;
        for (auto i = 0; i < 10; i++) {
            const auto newWidth = font->getPixelWidth(std::to_string(i));
            if (newWidth > widestWidth) {
                widestWidth = newWidth;
            }
        }
        return widestWidth;
    }
} // namespace

namespace gui
{
    namespace date_and_time = style::window::date_and_time;

    TimeFixedWidget::TimeFixedWidget(
        Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, const bool minus)
        : Rect(parent, x, y, w, h), minusVisible{minus}
    {
        setEdges(gui::RectangleEdge::None);
        hBox = new gui::HBox(this, 0, 0, w, h);
        hBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        firstHBox = new gui::HBox(hBox, 0, 0, 0, 0);
        firstHBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        firstHBox->setEdges(gui::RectangleEdge::None);

        colonText = new gui::Label(hBox, 0, 0, 0, 0);
        colonText->setEdges(gui::RectangleEdge::None);
        colonText->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonText->setText(colonSign);
        colonText->activeItem = false;

        secondHBox = new gui::HBox(hBox, 0, 0, 0, 0);
        secondHBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        secondHBox->setEdges(gui::RectangleEdge::None);

        minusText = new gui::Label(firstHBox, 0, 0, 0, 0);
        minusText->setEdges(gui::RectangleEdge::None);
        minusText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        setMinus(minus);

        firstPrimoText = new gui::Label(firstHBox, 0, 0, 0, 0);
        firstPrimoText->setEdges(gui::RectangleEdge::None);
        firstPrimoText->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        firstSecundoText = new gui::Label(firstHBox, 0, 0, 0, 0);
        firstSecundoText->setEdges(gui::RectangleEdge::None);
        firstSecundoText->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        secondPrimoText = new gui::Label(secondHBox, 0, 0, 0, 0);
        secondPrimoText->setEdges(gui::RectangleEdge::None);
        secondPrimoText->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        secondSecundoText = new gui::Label(secondHBox, 0, 0, 0, 0);
        secondSecundoText->setEdges(gui::RectangleEdge::None);
        secondSecundoText->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        hBox->resizeItems();
    }

    void TimeFixedWidget::setFirst(const uint32_t first)
    {
        const auto decimal = first / 10;
        if (decimal != 0u) {
            const auto widestDigit = getWidestDigitWidth(firstPrimoText->getTextFormat().getFont());
            firstPrimoText->setSize(widestDigit, hBox->widgetArea.h);
            firstPrimoText->setText(std::to_string(first / 10));
            firstSecundoText->setText(std::to_string(first % 10));
        }
        else {
            firstPrimoText->setText("");
            firstPrimoText->setSize(0, 0);
            firstSecundoText->setText(std::to_string(first));
        }
        firstHBox->resizeItems();
    }

    void TimeFixedWidget::setSecond(const uint32_t second)
    {
        const auto decimal = second / 10;
        if (decimal != 0u) {
            secondPrimoText->setText(std::to_string(second / 10));
            secondSecundoText->setText(std::to_string(second % 10));
        }
        else {
            secondPrimoText->setText("0");
            secondSecundoText->setText(std::to_string(second));
        }
    }

    void TimeFixedWidget::setMinus(const bool minus)
    {
        minusVisible = minus;
        minusText->setText(minus ? "-" : "");
        if (minusVisible) {
            const auto widestDigit = getWidestDigitWidth(minusText->getTextFormat().getFont());
            minusText->setSize(widestDigit, hBox->widgetArea.h);
        }
        else {
            minusText->setSize(0, 0);
        }
        firstHBox->resizeItems();
    }

    void TimeFixedWidget::setFont(const UTF8 &fontName)
    {
        minusText->setFont(fontName);
        firstPrimoText->setFont(fontName);
        firstSecundoText->setFont(fontName);
        colonText->setFont(fontName);
        secondPrimoText->setFont(fontName);
        secondSecundoText->setFont(fontName);

        const auto digitMaxWidth = getWidestDigitWidth(minusText->getTextFormat().getFont());
        const auto colonWidth    = minusText->getTextFormat().getFont()->getPixelWidth(colonSign);

        // Equal size for both boxes surrounding colon
        const auto boxesSize = (hBox->widgetArea.w - colonWidth) / 2;
        const auto height    = hBox->widgetArea.h;

        firstHBox->setSize(boxesSize, height);
        secondHBox->setSize(boxesSize, height);
        firstPrimoText->setSize(digitMaxWidth, height);
        firstSecundoText->setSize(digitMaxWidth, height);
        colonText->setSize(colonWidth, height);
        secondPrimoText->setSize(digitMaxWidth, height);
        secondSecundoText->setSize(digitMaxWidth, height);

        if (minusVisible) {
            minusText->setSize(digitMaxWidth, height);
        }
        else {
            minusText->setSize(0, 0);
        }
        hBox->resizeItems();
    }
} /* namespace gui */
