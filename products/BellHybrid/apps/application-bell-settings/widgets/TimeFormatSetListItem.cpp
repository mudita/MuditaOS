// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeFormatSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <gui/widgets/text/Label.hpp>

#include <widgets/TimeSetFmtSpinner.hpp>

namespace
{
    constexpr auto fmtSpinner12H = "12 h";
    constexpr auto fmtSpinner24H = "24 h";
} // namespace

namespace gui
{
    TimeFormatSetListItem::TimeFormatSetListItem(
        Length x, Length y, Length w, Length h, const UTF8 &topDesc, const UTF8 &botDesc)
    {
        setupTopTextBox(topDesc);
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        timeFormat = new StringSpinner({fmtSpinner12H, fmtSpinner24H}, Boundaries::Fixed);
        timeFormat->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        timeFormat->setFont(bell_settings_style::time_fmt_set_list_item::font);
        timeFormat->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeFormat->setFocusEdges(RectangleEdge::None);
        timeFormat->onValueChanged = [this](const auto &) {
            body->setMinMaxArrowsVisibility(timeFormat->is_min(), timeFormat->is_max());
        };

        body->getCenterBox()->addWidget(timeFormat);
        setupBottomTextBox(botDesc);
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

        inputCallback = [this](Item &, const InputEvent &inputEvent) -> bool {
            const auto ret = body->onInput(inputEvent);
            return ret;
        };
    }

    auto TimeFormatSetListItem::getTimeFmt() const noexcept -> utils::time::Locale::TimeFormat
    {
        return timeFormat->value() == fmtSpinner12H ? utils::time::Locale::TimeFormat::FormatTime12H
                                                    : utils::time::Locale::TimeFormat::FormatTime24H;
    }

    auto TimeFormatSetListItem::setTimeFmt(utils::time::Locale::TimeFormat fmt) noexcept -> void
    {
        using namespace utils::time;
        if (fmt == Locale::TimeFormat::FormatTime12H) {
            timeFormat->set_value(fmtSpinner12H);
        }
        else if (fmt == Locale::TimeFormat::FormatTime24H) {
            timeFormat->set_value(fmtSpinner24H);
        }
        body->setMinMaxArrowsVisibility(timeFormat->is_min(), timeFormat->is_max());
    }
} // namespace gui
