// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/spinners/Spinners.hpp>
#include <widgets/spinners/SpinnerContents.hpp>

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <i18n/i18n.hpp>

namespace app::bell_settings
{
    class SnoozeOnOffListItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        explicit SnoozeOnOffListItem(AbstractSnoozeSettingsModel &model)
            : BellSideListItemWithCallbacks(utils::translate("app_bell_settings_alarm_settings_snooze")),
              onStr{utils::translate("app_settings_toggle_on")}, offStr{utils::translate("app_settings_toggle_off")}
        {
            using namespace gui;
            spinner = new UTF8Spinner(UTF8Spinner::Range{offStr, onStr}, Boundaries::Fixed);
            spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
            spinner->setFont(bell_settings_style::time_fmt_set_list_item::font);
            spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            spinner->setFocusEdges(RectangleEdge::None);
            spinner->setCurrentValue(onStr);

            body->getCenterBox()->addWidget(spinner);

            getValue = [&model, this]() { model.setSnooze(isActive()); };
            setValue = [&model, this]() { spinner->setCurrentValue(model.getSnooze() ? onStr : offStr); };
        }

        bool isActive() const
        {
            return spinner->getCurrentValue() == onStr;
        }

      private:
        const UTF8 onStr;
        const UTF8 offStr;
        gui::UTF8Spinner *spinner{};
    };

    class SnoozeLengthListItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        explicit SnoozeLengthListItem(AbstractSnoozeSettingsModel &model)
            : BellSideListItemWithCallbacks(utils::translate("app_bell_settings_alarm_settings_snooze_length"))
        {
            using namespace gui;
            constexpr auto snoozeLengthStep = 1U;
            constexpr auto snoozeLengthMin  = 1U;
            constexpr auto snoozeLengthMax  = 30U;
            const auto bottomDescription    = utils::translate("common_minutes_lower");

            spinner = new UIntegerSpinner(UIntegerSpinner::Range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
                                          Boundaries::Fixed);
            spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
            spinner->setFont(bell_settings_style::time_fmt_set_list_item::font);
            spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            spinner->setFocusEdges(RectangleEdge::None);

            auto bottomText = new TextFixedSize(body->lastBox);
            bottomText->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
            bottomText->setFont(style::bell_sidelist_item::description_font);
            bottomText->setEdges(RectangleEdge::None);
            bottomText->activeItem = false;
            bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            bottomText->setText(bottomDescription);
            bottomText->drawUnderline(false);
            body->getCenterBox()->addWidget(spinner);

            getValue = [&model, this]() { model.setLength(spinner->getCurrentValue()); };
            setValue = [&model, this]() { spinner->setCurrentValue(model.getLength()); };
        }

      private:
        gui::UIntegerSpinner *spinner{};
    };

    class SnoozeChimeIntervalListItem : public gui::BellSideListItemWithCallbacks
    {
        using Value                = gui::NumWithString<std::uint32_t, UTF8>;
        using NumWithStringSpinner = gui::GenericSpinner<gui::StringPolicy<Value>>;

      public:
        explicit SnoozeChimeIntervalListItem(AbstractSnoozeSettingsModel &model)
            : BellSideListItemWithCallbacks(utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval")),
              offStr{utils::translate("app_settings_toggle_off")}, minStr{utils::translate("common_minute_short")}
        {
            using namespace gui;
            const auto bottomDescription =
                utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval_bot_desc");

            const auto range = NumWithStringSpinner::Range{
                Value{offStr}, Value{1, minStr}, Value{2, minStr}, Value{3, minStr}, Value{5, minStr}};

            spinner = new NumWithStringSpinner(range, Boundaries::Fixed);
            spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
            spinner->setFont(bell_settings_style::time_fmt_set_list_item::font);
            spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            spinner->setFocusEdges(RectangleEdge::None);
            body->getCenterBox()->addWidget(spinner);

            auto bottomText = new TextFixedSize(body->lastBox);
            bottomText->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
            bottomText->setFont(style::bell_sidelist_item::description_font);
            bottomText->setEdges(RectangleEdge::None);
            bottomText->activeItem = false;
            bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            bottomText->setText(bottomDescription);
            bottomText->drawUnderline(false);

            getValue = [&model, this]() {
                const auto val = spinner->getCurrentValue().getValue();
                model.setChimeInterval(not val ? 0 : *val);
            };
            setValue = [&model, this]() {
                const auto modelValue = model.getChimeInterval();
                if (modelValue > 0) {
                    spinner->setCurrentValue(Value{modelValue, minStr});
                }
                else {
                    spinner->setCurrentValue(Value{minStr});
                }
            };
        }

        bool isOff() const
        {
            return not spinner->getCurrentValue().getValue().has_value();
        }

      private:
        NumWithStringSpinner *spinner{};
        const UTF8 offStr;
        const UTF8 minStr;
    };

    class SnoozeChimeToneListItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        explicit SnoozeChimeToneListItem(AbstractSnoozeSettingsModel &model)
            : BellSideListItemWithCallbacks(utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone"))
        {
            using namespace gui;
            const auto textRange = UTF8Spinner::Range{{"Meditative\nsurprises"}}; // TODO: Full list needed

            spinner = new UTF8Spinner(textRange, Boundaries::Fixed);
            spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
            spinner->setFont(bell_settings_style::alarm_settings_chime_tone::font_center);
            spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            spinner->setFocusEdges(RectangleEdge::None);
            body->getCenterBox()->addWidget(spinner);

            getValue = [&model, this]() { model.setChimeTone(spinner->getCurrentValue()); };
            setValue = [&model, this]() { spinner->setCurrentValue(model.getChimeTone()); };
        }

      private:
        gui::UTF8Spinner *spinner{};
    };

    class SnoozeChimeVolumeListItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        explicit SnoozeChimeVolumeListItem(AbstractSnoozeSettingsModel &model)
            : BellSideListItemWithCallbacks(utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume"))
        {
            using namespace gui;
            constexpr auto volumeStep = 1U;
            constexpr auto volumeMin  = 1U;
            constexpr auto volumeMax  = 10U;

            spinner = new UIntegerSpinner(UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep}, Boundaries::Fixed);
            spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
            spinner->setFont(bell_settings_style::time_fmt_set_list_item::font);
            spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            spinner->setFocusEdges(RectangleEdge::None);
            body->getCenterBox()->addWidget(spinner);

            getValue = [&model, this]() { model.setChimeVolume(spinner->getCurrentValue()); };
            setValue = [&model, this]() { spinner->setCurrentValue(model.getChimeVolume()); };
        }

      private:
        gui::UIntegerSpinner *spinner{};
    };

} // namespace app::bell_settings
