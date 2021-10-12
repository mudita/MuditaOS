// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozeListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    SnoozeListItemProvider::SnoozeListItemProvider(AbstractSnoozeSettingsModel &model,
                                                   std::vector<UTF8> chimeTonesRange)
        : model{model}
    {
        buildListItems(chimeTonesRange);
    }

    void SnoozeListItemProvider::buildListItems(std::vector<UTF8> chimeTonesRange)
    {
        constexpr auto itemCount = 5U;
        internalData.reserve(itemCount);

        auto onOff =
            new OnOffListItem(model.getSnoozeOnOff(), utils::translate("app_bell_settings_alarm_settings_snooze"));
        onOff->onExit = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };
        internalData.emplace_back(onOff);
        constexpr auto snoozeLengthStep = 1U;
        constexpr auto snoozeLengthMin  = 1U;
        constexpr auto snoozeLengthMax  = 30U;

        auto chimeLengthBottomDescription = model.getSnoozeLength().getValue() > 1
                                                ? utils::translate("common_minutes_lower")
                                                : utils::translate("common_minute_lower");
        auto chimeLength                  = new NumListItem(model.getSnoozeLength(),
                                           UIntegerSpinner::Range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
                                           utils::translate("app_bell_settings_alarm_settings_snooze_length"),
                                           chimeLengthBottomDescription);

        chimeLength->setOnValueChanged([chimeLength](const std::uint32_t &val) {
            if (val == 1) {
                chimeLength->setBottomDescribtionText(utils::translate("common_minute_lower"));
            }
            else {
                chimeLength->setBottomDescribtionText(utils::translate("common_minutes_lower"));
            }
        });

        internalData.emplace_back(chimeLength);

        const UTF8 minStr = utils::translate("common_minute_short");
        const auto range  = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{1, minStr},
            NumWithStringListItem::Value{2, minStr},
            NumWithStringListItem::Value{3, minStr},
            NumWithStringListItem::Value{5, minStr}};
        auto chimeInterval = new NumWithStringListItem(
            model.getSnoozeChimeInterval(),
            range,
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval"),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval_bot_desc"));
        chimeInterval->onExit = [chimeInterval, this] {
            if (chimeInterval->isOff()) {
                this->onExit();
            }
        };
        internalData.emplace_back(chimeInterval);

        auto snoozeChimeTone = new UTF8ListItem(model.getSnoozeChimeTone(),
                                                std::move(chimeTonesRange),
                                                utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone"));
        snoozeChimeTone->setOnValueChanged([this](const UTF8 &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        snoozeChimeTone->onEnter = [this, snoozeChimeTone]() {
            if (onToneEnter) {
                onToneEnter(snoozeChimeTone->getCurrentValue());
            }
        };
        snoozeChimeTone->onExit = [this, snoozeChimeTone]() {
            if (onToneExit) {
                onToneExit(snoozeChimeTone->getCurrentValue());
            }
        };
        internalData.emplace_back(snoozeChimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto snoozeChimeVolume =
            new NumListItem(model.getSnoozeChimeVolume(),
                            UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                            utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume"));
        snoozeChimeVolume->setOnValueChanged([this](const UIntegerSpinner::Type &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        snoozeChimeVolume->onEnter = [this, snoozeChimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(snoozeChimeTone->getCurrentValue());
            }
        };

        snoozeChimeVolume->onExit = [this, snoozeChimeVolume]() {
            if (onVolumeExit) {
                onVolumeExit(snoozeChimeVolume->getCurrentValue());
            }
        };

        internalData.emplace_back(snoozeChimeVolume);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

} // namespace app::bell_settings
