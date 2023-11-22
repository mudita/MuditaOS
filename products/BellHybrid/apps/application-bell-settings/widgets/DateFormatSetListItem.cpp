// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateFormatSetListItem.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <gui/widgets/text/Label.hpp>

#include <widgets/TimeSetFmtSpinner.hpp>

namespace
{
    constexpr auto fmtSpinner_DD_MM = "DD / MM";
    constexpr auto fmtSpinner_MM_DD = "MM / DD";
} // namespace

namespace gui
{
    DateFormatSetListItem::DateFormatSetListItem(
        Length x, Length y, Length w, Length h, const UTF8 &topDesc, const UTF8 &botDesc)
    {
        setupTopTextBox(topDesc);
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        dateFormat = new StringSpinner({fmtSpinner_DD_MM, fmtSpinner_MM_DD}, Boundaries::Fixed);
        dateFormat->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        dateFormat->setFont(bell_settings_style::date_fmt_set_list_item::font);
        dateFormat->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        dateFormat->setFocusEdges(RectangleEdge::None);
        dateFormat->onValueChanged = [this](const auto &) {
            body->setMinMaxArrowsVisibility(dateFormat->is_min(), dateFormat->is_max());
        };

        body->getCenterBox()->addWidget(dateFormat);
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

    auto DateFormatSetListItem::getDateFmt() const noexcept -> utils::time::Locale::DateFormat
    {
        return (dateFormat->value() == fmtSpinner_DD_MM) ? utils::time::Locale::DateFormat::DD_MM_YYYY
                                                         : utils::time::Locale::DateFormat::MM_DD_YYYY;
    }

    auto DateFormatSetListItem::setDateFmt(utils::time::Locale::DateFormat fmt) noexcept -> void
    {
        using namespace utils::time;
        if (fmt == Locale::DateFormat::MM_DD_YYYY) {
            dateFormat->set_value(fmtSpinner_MM_DD);
        }
        else {
            dateFormat->set_value(fmtSpinner_DD_MM);
        }
        body->setMinMaxArrowsVisibility(dateFormat->is_min(), dateFormat->is_max());
    }
} // namespace gui
