// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "AlarmSettingsListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <common/widgets/list_items/Numeric.hpp>
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

        auto alarmTone = new list_items::Text(std::move(alarmTonesRange),
                                              model.getAlarmTone(),
                                              utils::translate("app_bell_settings_alarm_settings_tone"));
        alarmTone->set_on_value_change_cb([this](const auto &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        alarmTone->onEnter = [this, alarmTone]() {
            if (onToneEnter) {
                onToneEnter(alarmTone->value());
            }
        };
        alarmTone->onExit = [this, alarmTone]() {
            if (onToneExit) {
                onToneExit(alarmTone->value());
            }
        };
        internalData.emplace_back(alarmTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto alarmVolume =
            new list_items::Numeric(list_items::Numeric::spinner_type::range{volumeMin, volumeMax, volumeStep},
                                    model.getAlarmVolume(),
                                    utils::translate("app_bell_settings_alarm_settings_volume"));
        alarmVolume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });
        alarmVolume->onEnter = [this, alarmTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(alarmTone->value());
            }
        };
        alarmVolume->onExit = [this, alarmVolume]() {
            if (onVolumeExit) {
                onVolumeExit(alarmVolume->value());
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
