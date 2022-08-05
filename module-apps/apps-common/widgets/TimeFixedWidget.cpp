// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
    constexpr auto minusSign = "-";

    constexpr std::uint32_t getWidestDigitWidth(const gui::RawFont *const font)
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

    TimeFixedWidget::TimeFixedWidget(Item *parent,
                                     const std::uint32_t &x,
                                     const std::uint32_t &y,
                                     const std::uint32_t &w,
                                     const std::uint32_t &h,
                                     const bool minus)
        : Rect(parent, x, y, w, h), minusVisible{minus}
    {
        setEdges(gui::RectangleEdge::None);
        mainBox = new gui::HBox(this, 0, 0, w, h);
        mainBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        mainBox->setEdges(gui::RectangleEdge::None);
        mainBox->activeItem = false;

        leftBox.box = new gui::HBox(mainBox, 0, 0, 0, 0);
        leftBox.box->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        leftBox.box->setEdges(gui::RectangleEdge::None);

        attachLabelToBox(colon, mainBox);
        setColon();

        rightBox.box = new gui::HBox(mainBox, 0, 0, 0, 0);
        rightBox.box->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        rightBox.box->setEdges(gui::RectangleEdge::None);

        attachLabelToBox(leftBox.minus, leftBox.box);
        setMinus();

        for (auto &digit : leftBox.container.digits) {
            attachLabelToBox(digit, leftBox.box);
        }

        for (auto &digit : rightBox.container.digits) {
            attachLabelToBox(digit, rightBox.box);
        }

        mainBox->resizeItems();
    }

    void TimeFixedWidget::attachLabelToBox(Label *&label, HBox *&box) const
    {
        label = new gui::Label(box, 0, 0, 0, 0);
        label->setEdges(gui::RectangleEdge::None);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    }

    void TimeFixedWidget::setMinus() const
    {
        leftBox.minus->setText(minusVisible ? minusSign : "");
    }

    void TimeFixedWidget::setColon() const
    {
        colon->setText(colonSign);
        colon->activeItem = false;
    }

    void TimeFixedWidget::setMinutesBox(std::uint32_t minutes)
    {
        leftBox.container.setMinutesBox(minutes, getDimensions());
        leftBox.box->resizeItems();
    }

    void TimeFixedWidget::setSecondsBox(std::uint32_t seconds)
    {
        rightBox.container.setSecondsBox(seconds, getDimensions());
    }

    void TimeFixedWidget::setFontAndDimensions(const UTF8 &fontName) const
    {
        leftBox.minus->setFont(fontName);
        for (auto &digit : leftBox.container.digits) {
            digit->setFont(fontName);
        }

        colon->setFont(fontName);

        for (auto &digit : rightBox.container.digits) {
            digit->setFont(fontName);
        }

        setDimensions(getDimensions());
        mainBox->resizeItems();
    }

    void TimeFixedWidget::setDimensions(DimensionsParams &&params) const
    {

        leftBox.box->setSize(params.leftBoxWidth, params.mainBoxHeight);
        rightBox.box->setSize(params.rightBoxWidth, params.mainBoxHeight);

        leftBox.minus->setSize(params.minusWidth, params.mainBoxHeight);
        for (auto &digit : leftBox.container.digits) {
            digit->setSize(params.digitMaxWidth, params.mainBoxHeight);
        }

        colon->setSize(params.colonWidth, params.mainBoxHeight);

        for (auto &digit : rightBox.container.digits) {
            digit->setSize(params.digitMaxWidth, params.mainBoxHeight);
        }
    }

    DimensionsParams TimeFixedWidget::getDimensions() const
    {
        DimensionsParams info{};

        info.mainBoxHeight = mainBox->widgetArea.h;
        info.mainBoxWidth  = mainBox->widgetArea.w;

        info.digitMaxWidth = getWidestDigitWidth(leftBox.minus->getTextFormat().getFont());
        info.colonWidth    = leftBox.minus->getTextFormat().getFont()->getPixelWidth(colonSign);
        info.minusWidth    = leftBox.minus->getTextFormat().getFont()->getPixelWidth(minusSign);

        info.leftBoxWidth  = (info.digitMaxWidth * leftBox.container.digits.size()) + info.minusWidth;
        info.rightBoxWidth = info.mainBoxWidth - info.minusWidth - info.leftBoxWidth;

        return info;
    }

} /* namespace gui */
