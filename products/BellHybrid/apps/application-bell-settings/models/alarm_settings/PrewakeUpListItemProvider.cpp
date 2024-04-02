// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpListItemProvider.hpp"
#include "AbstractPrewakeUpSettingsModel.hpp"
#include "common/models/AbstractAudioModel.hpp"
#include <common/widgets/list_items/NumberWithSuffix.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <SongsListViewItem.hpp>
#include <Paths.hpp>

namespace app::bell_settings
{
    using namespace gui;

    PrewakeUpListItemProvider::PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &settingsModel,
                                                         std::shared_ptr<SongsModel> songsModel)
        : settingsModel{settingsModel}, songsModel{std::move(songsModel)}
    {
        buildListItems();
    }

    void PrewakeUpListItemProvider::buildListItems()
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 5U;
        internalData.reserve(itemCount);

        auto chimeDuration = new list_items::NumberWithSuffix(
            list_items::NumberWithSuffix::spinner_type::range{0, 5, 10, 15},
            settingsModel.getChimeDuration(),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_time_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_time_bottom_description"));

        chimeDuration->onProceed = [chimeDuration, this]() {
            if (chimeDuration->value() == 0) {
                constexpr auto lightDurationListIndex = 3U;
                list->rebuildList(gui::listview::RebuildType::OnOffset, lightDurationListIndex);
                return true;
            }
            return false;
        };

        internalData.emplace_back(chimeDuration);

        auto chimeTone =
            new SongsListViewItem(utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_top_description"),
                                  settingsModel.getChimeTone(),
                                  songsModel);

        currentSoundPath = settingsModel.getChimeTone().getValue();
        chimeTone->set_on_value_change_cb([this](const auto &val) {
            currentSoundPath = val;
            if (onToneChange) {
                onToneChange(currentSoundPath);
            }
        });

        chimeTone->onEnter = [this]() {
            if (onToneEnter) {
                onToneEnter(currentSoundPath);
            }
        };
        chimeTone->onExit = [this]() {
            if (onToneExit) {
                onToneExit(currentSoundPath);
            }
        };
        chimeTone->onProceed = [this]() {
            if (onToneProceed) {
                return onToneProceed(currentSoundPath);
            }
            return false;
        };
        internalData.emplace_back(chimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = AbstractAudioModel::minVolume;
        constexpr auto volumeMax  = AbstractAudioModel::maxVolume;
        prewakeUpVolume           = new list_items::NumericWithBar(
            list_items::NumericWithBar::spinner_type::range{volumeMin, volumeMax, volumeStep},
            settingsModel.getChimeVolume(),
            volumeMax,
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_volume"));
        prewakeUpVolume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        prewakeUpVolume->onEnter = [this, chimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(chimeTone->value());
            }
        };

        prewakeUpVolume->onExit = [this]() {
            if (onVolumeExit) {
                onVolumeExit(prewakeUpVolume->value());
            }
        };
        internalData.emplace_back(prewakeUpVolume);

        auto lightDuration = new list_items::NumberWithSuffix(
            list_items::NumberWithSuffix::spinner_type::range{0, 5, 10, 15},
            settingsModel.getLightDuration(),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_bottom_description"));

        lightDuration->onReturn = [this, chimeDuration]() {
            if (chimeDuration->value() == 0) {
                list->rebuildList(gui::listview::RebuildType::OnOffset, 0);
                return true;
            }
            return false;
        };

        lightDuration->onProceed = [this, lightDuration]() {
            if (lightDuration->value() == 0) {
                this->onExit();
                return true;
            }
            return false;
        };

        internalData.emplace_back(lightDuration);

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

    auto PrewakeUpListItemProvider::getCurrentVolume() -> std::uint8_t
    {
        return prewakeUpVolume->value();
    }
} // namespace app::bell_settings
