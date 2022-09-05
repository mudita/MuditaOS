// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        constexpr auto focusFontName   = style::window::font::large;
        constexpr auto noFocusFontName = style::window::font::largelight;

        void setFont(TextFixedSize *elem, const std::string &fontName)
        {
            elem->setFont(fontName);
            elem->setMinimumHeightToFitText();
            elem->setMinimumWidthToFitText();
            elem->setText(elem->getText());
        }

        class FocusHelper
        {
          public:
            FocusHelper &year()
            {
                year_ = focusFontName;
                return *this;
            }

            FocusHelper &month()
            {
                month_ = focusFontName;
                return *this;
            }

            FocusHelper &day()
            {
                day_ = focusFontName;
                return *this;
            }

            void focus(U8IntegerSpinnerFixed *elementDay,
                       U8IntegerSpinnerFixed *elementMonth,
                       U16IntegerSpinnerFixed *elementYear)
            {
                setFont(elementDay, day_);
                setFont(elementMonth, month_);
                setFont(elementYear, year_);
            }

          private:
            std::string year_{noFocusFontName};
            std::string month_{noFocusFontName};
            std::string day_{noFocusFontName};
        };
    } // namespace

    DateSetSpinner::DateSetSpinner(Item *parent, TextFixedSize *title, Length x, Length y, Length w, Length h)
        : HBox(parent, x, y, w, h), title{title}
    {
        constexpr std::uint8_t monthMin = 1;
        constexpr std::uint8_t monthMax = 12;

        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        attachDateField(
            year, U16IntegerSpinnerFixed::range{utils::time::Locale::min_years, utils::time::Locale::max_years, step});
        attachSlash(firstSlash);
        attachDateField(month, U8IntegerSpinnerFixed::range{monthMin, monthMax, step});
        attachSlash(secondSlash);
        attachDateField(day, U8IntegerSpinnerFixed::range{dayMin, dayMax, step});

        month->onValueChanged = [this](auto) { correctDayOfMonth(); };
        year->onValueChanged  = [this](auto) { correctDayOfMonth(); };

        resizeItems();

        focusChangedCallback = [&](Item &) {
            if (lastFocus != nullptr) {
                updateFocus(lastFocus);
            }
            return true;
        };

        updateFocus(year);
    }

    date::year_month_day DateSetSpinner::getDate()
    {
        return date::year(year->get_value()) / date::month(month->get_value()) / date::day(day->get_value());
    }

    void DateSetSpinner::setDate(const date::year_month_day date)
    {
        day->set_value(static_cast<unsigned>(date.day()));
        month->set_value(static_cast<unsigned>(date.month()));
        year->set_value(static_cast<int>(date.year()));
        applySizeRestrictions();
    }

    void DateSetSpinner::applySizeRestrictions()
    {
        day->setMinimumWidthToFitText();
        firstSlash->setMinimumWidthToFitText();
        month->setMinimumWidthToFitText();
        secondSlash->setMinimumWidthToFitText();
        year->setMinimumWidthToFitText();

        setMinimumSize(getWidgetMinimumAreaWidth(), getFontHeight(noFocusFontName));
        setMaximumWidth(widgetMaximumArea.w);

        HBox::informContentChanged();
    }

    std::uint32_t DateSetSpinner::getWidgetMinimumAreaWidth()
    {
        return day->widgetMinimumArea.w + firstSlash->widgetMinimumArea.w + month->widgetMinimumArea.w +
               secondSlash->widgetMinimumArea.w + year->widgetMinimumArea.w;
    }

    template <typename Spinner>
    void DateSetSpinner::attachDateField(Spinner *&field, typename Spinner::range &&range)
    {
        field = new Spinner(std::move(range), Boundaries::Continuous);
        setFont(field, noFocusFontName);
        field->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        field->setEdges(RectangleEdge::None);
        field->setPenFocusWidth(4);
        field->set_value(0);
        addWidget(field);
    }

    void DateSetSpinner::attachSlash(gui::Label *&slash)
    {
        slash = new gui::Label(this, 0, 0, 0, 0);
        setFont(slash, noFocusFontName);
        slash->setEdges(gui::RectangleEdge::None);
        slash->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        slash->setEdges(RectangleEdge::None);
        slash->setText("/");
        slash->activeItem = false;
    }

    std::uint16_t DateSetSpinner::getFontHeight(const std::string &fontName) const
    {
        const RawFont *font = FontManager::getInstance().getFont(fontName);
        return font->info.line_height;
    }

    bool DateSetSpinner::onInput(const InputEvent &inputEvent)
    {
        if (auto ret = this->focusItem->onInput(inputEvent)) {
            applySizeRestrictions();
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

    void DateSetSpinner::correctDayOfMonth()
    {
        const auto dayCountInMonth =
            static_cast<unsigned>((date::year(year->get_value()) / date::month(month->get_value()) / date::last).day());

        if (day->get_value() > dayCountInMonth) {
            day->set_value(dayCountInMonth);
        }

        const auto currentValue = day->get_value();
        day->set_range({dayMin, static_cast<std::uint8_t>(dayCountInMonth), step});
        day->set_value(currentValue);
    }

    bool DateSetSpinner::handleEnterKey()
    {
        if (focusItem == year) {
            updateFocus(month);
            return true;
        }
        if (focusItem == month) {
            updateFocus(day);
            return true;
        }
        return false;
    }

    bool DateSetSpinner::handleRightFunctionKey()
    {
        if (focusItem == month) {
            updateFocus(year);
            return true;
        }
        if (focusItem == day) {
            updateFocus(month);
            return true;
        }
        return false;
    }

    void DateSetSpinner::updateFocus(Item *newFocus)
    {
        auto set_title = [this](std::string text) {
            if (title != nullptr) {
                title->setRichText(text);
            }
        };

        setFocusItem(newFocus);
        lastFocus = newFocus;

        if (month->focus) {
            set_title(utils::translate("app_settings_title_month"));
            FocusHelper{}.month().focus(day, month, year);
        }
        else if (day->focus) {
            set_title(utils::translate("app_settings_title_day"));
            FocusHelper{}.day().focus(day, month, year);
        }
        else if (year->focus) {
            set_title(utils::translate("app_settings_title_year"));
            FocusHelper{}.year().focus(day, month, year);
        }

        applySizeRestrictions();
    }
} /* namespace gui */
