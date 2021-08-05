// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TimeSetFmtSpinner.hpp"
#include "TimeSetSpinner.hpp"

#include <date/date.h>
#include <gui/core/FontManager.hpp>
#include <gui/core/RawFont.hpp>
#include <gui/widgets/Spinner.hpp>
#include <gui/widgets/TextSpinner.hpp>

namespace
{
    constexpr auto fmtSpinnerAMPos = 0U;
    constexpr auto fmtSpinnerPMPos = 1U;
} // namespace

namespace gui
{

    TimeSetFmtSpinner::TimeSetFmtSpinner(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : HBox{parent, x, y, w, h}
    {
        using namespace utils;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        timeSetSpinner = new TimeSetSpinner(this, 0, 0, 0, 0);
        timeSetSpinner->setFont(fontName);
        timeSetSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeSetSpinner->setMargins(Margins(0, 0, 0, 0));

        auto minSize   = getMinimumFmtSize();
        auto textRange = TextSpinner::TextRange{time::Locale::getAM(), time::Locale::getPM()};
        fmt            = new TextSpinner(textRange, Boundaries::Continuous);
        fmt->setMinimumSize(minSize.first, minSize.second);
        fmt->setFont(fontName);
        fmt->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        fmt->setEdges(RectangleEdge::None);
        fmt->setVisible(false);
        fmt->setPenFocusWidth(style::time_set_spinner::focus::size);
        addWidget(fmt);
        fmt->setEdges(RectangleEdge::Bottom);

        focusChangedCallback = [&](Item &) {
            if (focus && editMode != EditMode::Browse) {
                setFocusItem(timeSetSpinner);
            }
            else {
                setFocusItem(nullptr);
            }
            setTimeFormat(this->timeFormat);
            return true;
        };

        setTimeFormat(timeFormat);
    }

    auto TimeSetFmtSpinner::setTimeFormat(utils::time::Locale::TimeFormat newFormat) noexcept -> void
    {
        using namespace utils;
        timeFormat = newFormat;
        switch (timeFormat) {
        case utils::time::Locale::TimeFormat::FormatTime12H: {
            fmt->setVisible(true);
            timeSetSpinner->setHourMax(time::Locale::max_hour_12H_mode);
            timeSetSpinner->setHourMin(time::Locale::min_hour_12H_mode);

            auto hours = std::chrono::hours(timeSetSpinner->getHour());
            timeSetSpinner->setHour(date::make12(hours).count());
            if (date::is_pm(hours)) {
                fmt->setCurrentPosition(fmtSpinnerPMPos);
            }
            else {
                fmt->setCurrentPosition(fmtSpinnerAMPos);
            }
        } break;
        case utils::time::Locale::TimeFormat::FormatTime24H: {
            fmt->setVisible(false);
            timeSetSpinner->setHourMax(time::Locale::max_hour_24H_mode);
            timeSetSpinner->setHourMin(time::Locale::min_hour_24H_mode);

            auto hours = std::chrono::hours(timeSetSpinner->getHour());
            timeSetSpinner->setHour(date::make24(hours, isPm(fmt->getCurrentText())).count());
        } break;
        default:
            break;
        }

        resizeItems();

        // If we make 12->24 switch while focused on fmt then change focus to hour
        if (focusItem == fmt) {
            setFocusItem(timeSetSpinner);
        }
    }

    auto TimeSetFmtSpinner::setMinute(int value) noexcept -> void
    {
        timeSetSpinner->setMinute(value);
    }

    auto TimeSetFmtSpinner::setEditMode(EditMode newEditMode) noexcept -> void
    {
        editMode = newEditMode;
        if (editMode == EditMode::Edit) {
            setFocusItem(timeSetSpinner);
        }
        else {
            setFocusItem(nullptr);
        }
    }
    auto TimeSetFmtSpinner::getHour() const noexcept -> int
    {
        return timeSetSpinner->getHour();
    }
    auto TimeSetFmtSpinner::getMinute() const noexcept -> int
    {
        return timeSetSpinner->getMinute();
    }

    auto TimeSetFmtSpinner::getFontHeight() const noexcept -> uint16_t
    {
        const auto font = FontManager::getInstance().getFont(fontName);
        return font->info.line_height;
    }
    auto TimeSetFmtSpinner::getMinimumFmtSize() const noexcept -> std::pair<Length, Length>
    {
        constexpr auto spacer = 5U; // space between two chars
        const auto font       = FontManager::getInstance().getFont(fontName);
        return {font->getPixelWidth(utils::time::Locale::getAM()) + spacer, font->info.line_height};
    }
    auto TimeSetFmtSpinner::setHour(int value) noexcept -> void
    {
        timeSetSpinner->setHour(value);
    }
    auto TimeSetFmtSpinner::setFont(std::string newFontName) noexcept -> void
    {
        fontName        = std::move(newFontName);
        auto fontHeight = getFontHeight();
        auto minFmtSize = getMinimumFmtSize();

        timeSetSpinner->setFont(fontName);
        fmt->setFont(fontName);
        fmt->setMinimumSize(minFmtSize.first, minFmtSize.second);
        fmt->setText(fmt->getText());

        setMinimumSize(timeSetSpinner->getMinimumSize().first + minFmtSize.first, fontHeight);
        resizeItems();
    }

    auto TimeSetFmtSpinner::onInput(const InputEvent &inputEvent) -> bool
    {
        // Ignore input event when not in edit mode
        if (editMode != EditMode::Edit) {
            return false;
        }

        if (auto ret = this->focusItem->onInput(inputEvent)) {
            return ret;
        }

        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_ENTER:
                return handleEnterKey();
            case KeyCode::KEY_RF:
                return handleRightFunctionKey();

            default:
                break;
            }
        }
        return false;
    }
    auto TimeSetFmtSpinner::handleEnterKey() -> bool
    {
        using namespace utils::time;
        if ((focusItem == timeSetSpinner) && (timeFormat == Locale::TimeFormat::FormatTime12H)) {
            setFocusItem(fmt);
            return true;
        }
        return false;
    }
    auto TimeSetFmtSpinner::handleRightFunctionKey() -> bool
    {
        if (focusItem == fmt) {
            setFocusItem(timeSetSpinner);
            return true;
        }

        return false;
    }
    auto TimeSetFmtSpinner::isPm(const std::string_view str) const noexcept -> bool
    {
        return str == utils::time::Locale::getPM().c_str();
    }
} // namespace gui