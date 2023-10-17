// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "AlarmSettingsListItemProvider.hpp"
#include <common/models/FrontlightModel.hpp>
#include <common/widgets/ListItems.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <apps-common/ApplicationCommon.hpp>

namespace app::bell_settings
{
    using namespace gui;

    AlarmSettingsListItemProvider::AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &settingsModel,
                                                                 std::vector<UTF8> alarmToneRange)
        : settingsModel{settingsModel}
    {
        buildListItems(std::move(alarmToneRange));
    }

    void AlarmSettingsListItemProvider::buildListItems(std::vector<UTF8> alarmTonesRange)
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 5U;
        internalData.reserve(itemCount);

        auto alarmTone = new list_items::Text(std::move(alarmTonesRange),
                                              settingsModel.getAlarmTone(),
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
                                    settingsModel.getAlarmVolume(),
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

        auto onOffLight = new OnOffListItem(settingsModel.getAlarmLightOnOff(),
                                            utils::translate("app_bell_settings_alarm_settings_light"));

        internalData.emplace_back(onOffLight);

        onOffLight->onProceed = [onOffLight, this]() {
            if (!onOffLight->isActive()) {
                this->onExit();
                return true;
            }
            return false;
        };

        auto brightness = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{brightnessMin, brightnessMax, brightnessStep},
            settingsModel.getBrightness(),
            utils::translate("app_bell_settings_frontlight_top_message"));

        brightness->set_on_value_change_cb([this](const auto &val) {
            if (onFrontlightChange) {
                onFrontlightChange(val);
            }
        });

        brightness->onEnter = [this, brightness]() {
            if (onFrontlightEnter) {
                onFrontlightEnter(brightness->value());
            }
        };

        brightness->onExit = [this]() {
            if (onFrontlightExit) {
                onFrontlightExit();
            }
        };

        internalData.emplace_back(brightness);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }
} // namespace app::bell_settings
