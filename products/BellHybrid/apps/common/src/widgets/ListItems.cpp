// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/data/StyleCommon.hpp>
#include <common/widgets/ListItems.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace gui
{
    OnOffListItem::OnOffListItem(AbstractSettingsModel<bool> &model, const std::string &topDescription)
        : BellSideListItemWithCallbacks(topDescription), onStr{utils::translate("app_settings_toggle_on")},
          offStr{utils::translate("app_settings_toggle_off")}
    {
        spinner = new UTF8Spinner(UTF8Spinner::Range{offStr, onStr}, Boundaries::Fixed);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(bell_style::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->setCurrentValue(onStr);
        body->getCenterBox()->addWidget(spinner);

        getValue = [&model, this]() { model.setValue(isActive()); };
        setValue = [&model, this]() {
            spinner->setCurrentValue(model.getValue() ? onStr : offStr);
            setArrowsVisibility();
        };
        inputCallback = [this](Item &, const InputEvent &event) {
            const auto result = OnInputCallback(event);
            setArrowsVisibility();
            return result;
        };
    }

    bool OnOffListItem::isActive() const
    {
        return spinner->getCurrentValue() == onStr;
    }
    void OnOffListItem::setArrowsVisibility()
    {
        const auto selectedVal = spinner->getCurrentValue();
        body->setMinMaxArrowsVisibility(selectedVal == offStr, selectedVal == onStr);
    }

    NumListItem::NumListItem(AbstractSettingsModel<std::uint8_t> &model,
                             UIntegerSpinner::Range range,
                             const std::string &topDescription,
                             const std::string &bottomDescription)
        : BellSideListItemWithCallbacks(topDescription)
    {
        spinner = new UIntegerSpinner(range, Boundaries::Fixed);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(bell_style::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        setupBottomDescription(bottomDescription);

        getValue = [&model, this]() { model.setValue(spinner->getCurrentValue()); };
        setValue = [&model, this, range]() {
            spinner->setCurrentValue(model.getValue());
            setArrowsVisibility(range);
        };

        inputCallback = [&, range](Item &item, const InputEvent &event) {
            const auto result = OnInputCallback(event);
            setArrowsVisibility(range);
            return result;
        };
    }
    void NumListItem::setOnValueChanged(std::function<void(const UIntegerSpinner::Type &)> &&cb)
    {
        spinner->onValueChanged = cb;
    }
    UIntegerSpinner::Type NumListItem::getCurrentValue()
    {
        return spinner->getCurrentValue();
    }

    void NumListItem::setArrowsVisibility(UIntegerSpinner::Range range)
    {
        const auto selectedVal = spinner->getCurrentValue();
        body->setMinMaxArrowsVisibility(selectedVal == range.min, selectedVal == range.max);
    }

    NumWithStringListItem::NumWithStringListItem(AbstractSettingsModel<std::uint8_t> &model,
                                                 NumWithStringSpinner::Range range,
                                                 const std::string &topDescription,
                                                 const std::string &bottomDescription)
        : BellSideListItemWithCallbacks(topDescription), minStr{utils::translate("common_minute_short")}
    {

        spinner = new NumWithStringSpinner(range, Boundaries::Fixed);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(bell_style::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        setupBottomDescription(bottomDescription);

        inputCallback = [&, range](Item &item, const InputEvent &event) {
            const auto result = OnInputCallback(event);
            bottomText->setVisible(spinner->getCurrentValue().getValue().has_value());
            setArrowsVisibility(range);
            return result;
        };

        focusChangedCallback = [&](Item &) {
            bottomText->setVisible(spinner->getCurrentValue().getValue().has_value());
            OnFocusChangedCallback();
            return true;
        };

        getValue = [&model, this]() {
            const auto val = spinner->getCurrentValue().getValue();
            model.setValue(not val ? 0 : *val);
        };
        setValue = [&model, this, range]() {
            const auto modelValue = model.getValue();
            if (modelValue > 0) {
                spinner->setCurrentValue(Value{modelValue, minStr});
            }
            else {
                spinner->setCurrentValue(Value{minStr});
            }
            setArrowsVisibility(range);
        };
    }

    bool NumWithStringListItem::isOff() const
    {
        return not spinner->getCurrentValue().getValue().has_value();
    }

    void NumWithStringListItem::setArrowsVisibility(const NumWithStringSpinner::Range &range)
    {
        const auto selectedVal = spinner->getCurrentValue();
        body->setMinMaxArrowsVisibility(selectedVal == range.front(), selectedVal == range.back());
    }

    UTF8ListItem::UTF8ListItem(AbstractSettingsModel<UTF8> &model,
                               UTF8Spinner::Range range,
                               const std::string &topDescription)
        : BellSideListItemWithCallbacks(topDescription)
    {
        spinner = new UTF8Spinner(range, Boundaries::Fixed);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(bell_style::font_center);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        getValue = [&model, this]() { model.setValue(spinner->getCurrentValue()); };
        setValue = [&model, this, range]() {
            spinner->setCurrentValue(model.getValue());
            setArrowsVisibility(range);
        };
        inputCallback = [&, range](Item &item, const InputEvent &event) {
            const auto result = OnInputCallback(event);
            setArrowsVisibility(range);
            return result;
        };
    }

    void UTF8ListItem::setOnValueChanged(std::function<void(const UTF8 &)> &&cb)
    {
        spinner->onValueChanged = cb;
    }

    UTF8Spinner::Type UTF8ListItem::getCurrentValue()
    {
        return spinner->getCurrentValue();
    }
    void UTF8ListItem::setArrowsVisibility(const UTF8Spinner::Range &range)
    {
        const auto selectedVal = spinner->getCurrentValue();
        body->setMinMaxArrowsVisibility(selectedVal == range.front(), selectedVal == range.back());
    }

    TimeListItem::TimeListItem(AbstractSettingsModel<time_t> &model,
                               utils::time::Locale::TimeFormat timeFormat,
                               const std::string &focusFont,
                               const std::string &noFocusFont,
                               const std::string &topDescription)
        : BellSideListItemWithCallbacks(topDescription)
    {
        auto centerBox = dynamic_cast<HBox *>(body->getCenterBox());
        spinner        = new TimeSetFmtSpinner(centerBox);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(focusFont, noFocusFont);
        spinner->setTimeFormat(timeFormat);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        getValue = [&model, this]() { model.setValue(spinner->getTime()); };
        setValue = [&model, this]() { spinner->setTime(model.getValue()); };
    }
} // namespace gui
