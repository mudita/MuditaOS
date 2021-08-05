// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeFormatSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Spinner.hpp>

#include <widgets/TimeSetFmtSpinner.hpp>

namespace
{
    constexpr auto fmtSpinnerMin  = 12U;
    constexpr auto fmtSpinnerMax  = 24U;
    constexpr auto fmtSpinnerStep = 12U;

    uint16_t getFontHeight()
    {
        const gui::RawFont *font = gui::FontManager::getInstance().getFont(style::window::font::supersizemelight);
        return font->info.line_height;
    }
} // namespace

namespace gui
{

    TimeFormatSetListItem::TimeFormatSetListItem(
        Length x, Length y, Length w, Length h, const UTF8 &topDesc, const UTF8 &botDesc)
        : SideListItem(topDesc)
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        timeFormat = new Spinner(fmtSpinnerMin, fmtSpinnerMax, fmtSpinnerStep, Boundaries::Continuous);
        timeFormat->setMinimumSize(bell_settings_style::time_fmt_set_list_item::w, getFontHeight());
        timeFormat->setFont(bell_settings_style::time_fmt_set_list_item::font);

        timeFormat->setMargins(calculateMargins());
        timeFormat->setAlignment(Alignment(Alignment::Horizontal::Center));
        timeFormat->setFixedFieldWidth(2);
        timeFormat->setEdges(RectangleEdge::None);
        timeFormat->setCurrentValue(fmtSpinnerMin);
        timeFormat->setFocusEdges(RectangleEdge::None);
        body->addWidget(timeFormat);

        bottomDescription = new Label(body);
        bottomDescription->setMinimumSize(style::sidelistview::top_message::w, style::sidelistview::top_message::h);
        bottomDescription->setFont(style::sidelistview::top_message::font);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center));
        bottomDescription->setText(botDesc);

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        focusChangedCallback = [&](Item &) {
            setFocusItem(focus ? body : nullptr);
            if (focus) {
                setFocusItem(body);
            }
            else {
                setFocusItem(nullptr);
                if (onNextCallback) {
                    onNextCallback(*this);
                }
            }
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }
    auto TimeFormatSetListItem::getTimeFmt() const noexcept -> utils::time::Locale::TimeFormat
    {
        return timeFormat->getCurrentValue() == fmtSpinnerMin ? utils::time::Locale::TimeFormat::FormatTime12H
                                                              : utils::time::Locale::TimeFormat::FormatTime24H;
    }
    Margins TimeFormatSetListItem::calculateMargins() const noexcept
    {
        constexpr Position availableHeight =
            style::sidelistview::list_item::h -
            (bell_settings_style::time_fmt_set_list_item::h + (2 * style::sidelistview::top_message::h));
        return Margins{0, availableHeight / 2, 0, availableHeight / 4};
    }
    auto TimeFormatSetListItem::setTimeFmt(utils::time::Locale::TimeFormat fmt) noexcept -> void
    {
        using namespace utils::time;
        if (fmt == Locale::TimeFormat::FormatTime12H) {
            timeFormat->setCurrentValue(fmtSpinnerMin);
        }
        else if (fmt == Locale::TimeFormat::FormatTime24H) {
            timeFormat->setCurrentValue(fmtSpinnerMax);
        }
    }
} // namespace gui