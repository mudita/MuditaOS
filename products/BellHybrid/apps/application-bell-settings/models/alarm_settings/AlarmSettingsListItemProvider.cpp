// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "AlarmSettingsListItemProvider.hpp"
#include "common/models/AbstractAudioModel.hpp"
#include <common/models/FrontlightModel.hpp>
#include <common/widgets/ListItems.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <SongsListViewItem.hpp>

namespace app::bell_settings
{
    using namespace gui;

    AlarmSettingsListItemProvider::AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &settingsModel,
                                                                 std::shared_ptr<SongsModel> songsModel)
        : settingsModel{settingsModel}, songsModel{std::move(songsModel)}
    {
        buildListItems();
    }

    void AlarmSettingsListItemProvider::buildListItems()
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 5U;
        internalData.reserve(itemCount);

        auto alarmTone = new SongsListViewItem(
            utils::translate("app_bell_settings_alarm_settings_chime"), settingsModel.getAlarmTone(), songsModel);

        currentSoundPath = settingsModel.getAlarmTone().getValue();
        alarmTone->set_on_value_change_cb([this](const auto &val) {
            currentSoundPath = val;
            if (onToneChange) {
                onToneChange(currentSoundPath);
            }
        });
        alarmTone->onEnter = [this]() {
            if (onToneEnter) {
                onToneEnter(currentSoundPath);
            }
        };
        alarmTone->onExit = [this]() {
            if (onToneExit) {
                onToneExit(currentSoundPath);
            }
        };
        alarmTone->onProceed = [this]() {
            if (onToneProceed) {
                return onToneProceed(currentSoundPath);
            }
            return false;
        };
        internalData.emplace_back(alarmTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = AbstractAudioModel::minVolume;
        constexpr auto volumeMax  = AbstractAudioModel::maxVolume;
        alarmVolume               = new list_items::NumericWithBar(
            list_items::NumericWithBar::spinner_type::range{volumeMin, volumeMax, volumeStep},
            settingsModel.getAlarmVolume(),
            volumeMax,
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
        alarmVolume->onExit = [this]() {
            if (onVolumeExit) {
                onVolumeExit(alarmVolume->value());
            }
        };

        internalData.emplace_back(alarmVolume);

        auto onOffFade = new OnOffListItem(settingsModel.getAlarmFadeOnOff(),
                                           utils::translate("app_bell_settings_alarm_settings_fade"));

        internalData.emplace_back(onOffFade);

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

    auto AlarmSettingsListItemProvider::getCurrentVolume() -> std::uint8_t
    {
        return alarmVolume->value();
    }
} // namespace app::bell_settings
