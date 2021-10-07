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
        setValue = [&model, this]() { spinner->setCurrentValue(model.getValue() ? onStr : offStr); };
    }

    bool OnOffListItem::isActive() const
    {
        return spinner->getCurrentValue() == onStr;
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

        auto bottomText = new TextFixedSize(body->lastBox);
        bottomText->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::outer_layouts_h);
        bottomText->setFont(::style::bell_sidelist_item::description_font);
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->setText(bottomDescription);
        bottomText->drawUnderline(false);
        body->getCenterBox()->addWidget(spinner);

        getValue = [&model, this]() { model.setValue(spinner->getCurrentValue()); };
        setValue = [&model, this]() { spinner->setCurrentValue(model.getValue()); };
    }
    void NumListItem::setOnValueChanged(std::function<void(const UIntegerSpinner::Type &)> &&cb)
    {
        spinner->onValueChanged = cb;
    }
    UIntegerSpinner::Type NumListItem::getCurrentValue()
    {
        return spinner->getCurrentValue();
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

        bottomText = new TextFixedSize(body->lastBox);
        bottomText->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::outer_layouts_h);
        bottomText->setFont(::style::bell_sidelist_item::description_font);
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->setText(bottomDescription);
        bottomText->drawUnderline(false);

        inputCallback = [&](Item &item, const InputEvent &event) {
            const auto result = body->onInput(event);
            bottomText->setVisible(spinner->getCurrentValue().getValue().has_value());
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
        setValue = [&model, this]() {
            const auto modelValue = model.getValue();
            if (modelValue > 0) {
                spinner->setCurrentValue(Value{modelValue, minStr});
            }
            else {
                spinner->setCurrentValue(Value{minStr});
            }
        };
    }

    bool NumWithStringListItem::isOff() const
    {
        return not spinner->getCurrentValue().getValue().has_value();
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
        setValue = [&model, this]() { spinner->setCurrentValue(model.getValue()); };
    }
    void UTF8ListItem::setOnValueChanged(std::function<void(const UTF8 &)> &&cb)
    {
        spinner->onValueChanged = cb;
    }
    UTF8Spinner::Type UTF8ListItem::getCurrentValue()
    {
        return spinner->getCurrentValue();
    }
} // namespace gui
