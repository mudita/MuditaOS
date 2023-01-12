// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        spinner = new StringSpinner(StringSpinner::range{offStr, onStr}, Boundaries::Fixed);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(bell_style::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->set_value(onStr);
        body->getCenterBox()->addWidget(spinner);

        getValue = [&model, this]() { model.setValue(isActive()); };
        setValue = [&model, this]() {
            spinner->set_value(model.getValue() ? onStr : offStr);
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
        return spinner->value() == onStr;
    }
    void OnOffListItem::setArrowsVisibility()
    {
        body->setMinMaxArrowsVisibility(spinner->is_min(), spinner->is_max());
    }

    TimeListItem::TimeListItem(AbstractSettingsModel<time_t> &model,
                               utils::time::Locale::TimeFormat timeFormat,
                               const std::string &focusFont,
                               const std::string &noFocusFont,
                               const std::string &topDescription)
        : BellSideListItemWithCallbacks(topDescription)
    {
        spinner = new TimeSetFmtSpinner(body->getCenterBox());
        spinner->setTimeFormat(timeFormat);
        spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
        spinner->setFont(focusFont, noFocusFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        getValue = [&model, this]() { model.setValue(spinner->getTime()); };
        setValue = [&model, this]() { spinner->setTime(model.getValue()); };
    }
} // namespace gui
