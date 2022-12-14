// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpListItemProvider.hpp"
#include <common/models/FrontlightModel.hpp>
#include <common/widgets/list_items/NumberWithSuffix.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/ListItems.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    PrewakeUpListItemProvider::PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &settingsModel,
                                                         std::vector<UTF8> chimeToneRange)
        : settingsModel{settingsModel}
    {
        buildListItems(std::move(chimeToneRange));
    }

    void PrewakeUpListItemProvider::buildListItems(std::vector<UTF8> chimeToneRange)
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 5U;
        internalData.reserve(itemCount);

        const std::string minStr = utils::translate("common_minute_short");
        auto chimeDuration       = new list_items::NumberWithSuffix(
            list_items::NumberWithSuffix::spinner_type::range{0, 5, 10, 15},
            settingsModel.getChimeDuration(),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_bottom_description"));

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
            new list_items::Text(std::move(chimeToneRange),
                                 settingsModel.getChimeTone(),
                                 utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_tone"));
        chimeTone->set_on_value_change_cb([this](const auto &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        chimeTone->onEnter = [this, chimeTone]() {
            if (onToneEnter) {
                onToneEnter(chimeTone->value());
            }
        };
        chimeTone->onExit = [this, chimeTone]() {
            if (onToneExit) {
                onToneExit(chimeTone->value());
            }
        };
        internalData.emplace_back(chimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto volume =
            new list_items::Numeric(list_items::Numeric::spinner_type::range{volumeMin, volumeMax, volumeStep},
                                    settingsModel.getChimeVolume(),
                                    utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_volume"));
        volume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        volume->onEnter = [this, chimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(chimeTone->value());
            }
        };

        volume->onExit = [this, volume]() {
            if (onVolumeExit) {
                onVolumeExit(volume->value());
            }
        };
        internalData.emplace_back(volume);

        auto lightDuration = new list_items::NumberWithSuffix(
            list_items::NumberWithSuffix::spinner_type::range{0, 5, 10, 15},
            settingsModel.getLightDuration(),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_bottom_description"));

        lightDuration->onReturn = [chimeDuration, this]() {
            if (chimeDuration->value() == 0) {
                list->rebuildList(gui::listview::RebuildType::OnOffset, 0);
                return true;
            }
            return false;
        };

        lightDuration->onProceed = [lightDuration, this]() {
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
} // namespace app::bell_settings
