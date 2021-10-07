// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "AlarmSettingsListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>
#include <apps-common/ApplicationCommon.hpp>

namespace app::bell_settings
{
    using namespace gui;

    AlarmSettingsListItemProvider::AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &model,
                                                                 std::vector<UTF8> alarmToneRange)
        : model{model}
    {
        buildListItems(std::move(alarmToneRange));
    }

    void AlarmSettingsListItemProvider::buildListItems(std::vector<UTF8> alarmTonesRange)
    {
        constexpr auto itemCount = 4U;
        internalData.reserve(itemCount);

        auto alarmTone = new UTF8ListItem(model.getAlarmTone(),
                                          std::move(alarmTonesRange),
                                          utils::translate("app_bell_settings_alarm_settings_tone"));
        alarmTone->setOnValueChanged([this](const UTF8 &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        alarmTone->onEnter = [this, alarmTone]() {
            if (onToneEnter) {
                onToneEnter(alarmTone->getCurrentValue());
            }
        };
        alarmTone->onExit = [this, alarmTone]() {
            if (onToneExit) {
                onToneExit(alarmTone->getCurrentValue());
            }
        };
        internalData.emplace_back(alarmTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto alarmVolume          = new NumListItem(model.getAlarmVolume(),
                                           UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                                           utils::translate("app_bell_settings_alarm_settings_volume"));
        alarmVolume->setOnValueChanged([this](const UIntegerSpinner::Type &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });
        alarmVolume->onEnter = [this, alarmTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(alarmTone->getCurrentValue());
            }
        };
        alarmVolume->onExit = [this, alarmVolume]() {
            if (onVolumeExit) {
                onVolumeExit(alarmVolume->getCurrentValue());
            }
        };

        internalData.emplace_back(alarmVolume);

        internalData.emplace_back(
            new OnOffListItem(model.getAlarmLightOnOff(), utils::translate("app_bell_settings_alarm_settings_light")));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }
} // namespace app::bell_settings
