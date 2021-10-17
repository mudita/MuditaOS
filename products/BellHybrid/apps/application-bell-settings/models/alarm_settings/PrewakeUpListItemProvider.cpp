// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    PrewakeUpListItemProvider::PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &model,
                                                         std::vector<UTF8> chimeToneRange)
        : model{model}
    {
        buildListItems(std::move(chimeToneRange));
    }

    void PrewakeUpListItemProvider::buildListItems(std::vector<UTF8> chimeToneRange)
    {
        constexpr auto itemCount = 4U;
        internalData.reserve(itemCount);

        const UTF8 minStr             = utils::translate("common_minute_short");
        const auto chimeDurationRange = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{5, minStr},
            NumWithStringListItem::Value{10, minStr},
            NumWithStringListItem::Value{15, minStr}};
        auto chimeDuration = new NumWithStringListItem(
            model.getChimeDuration(),
            chimeDurationRange,
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_bottom_description"));

        chimeDuration->onProceed = [chimeDuration, this]() {
            if (chimeDuration->isOff()) {
                constexpr auto lightDurationListIndex = 3U;
                list->rebuildList(gui::listview::RebuildType::OnOffset, lightDurationListIndex);
                return true;
            }
            return false;
        };

        internalData.emplace_back(chimeDuration);

        auto chimeTone = new UTF8ListItem(model.getChimeTone(),
                                          std::move(chimeToneRange),
                                          utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_tone"));
        chimeTone->setOnValueChanged([this](const UTF8 &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        chimeTone->onEnter = [this, chimeTone]() {
            if (onToneEnter) {
                onToneEnter(chimeTone->getCurrentValue());
            }
        };
        chimeTone->onExit = [this, chimeTone]() {
            if (onToneExit) {
                onToneExit(chimeTone->getCurrentValue());
            }
        };
        internalData.emplace_back(chimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto volume               = new NumListItem(model.getChimeVolume(),
                                      UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                                      utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_volume"));
        volume->setOnValueChanged([this](const UIntegerSpinner::Type &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        volume->onEnter = [this, chimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(chimeTone->getCurrentValue());
            }
        };

        volume->onExit = [this, volume]() {
            if (onVolumeExit) {
                onVolumeExit(volume->getCurrentValue());
            }
        };
        internalData.emplace_back(volume);

        const auto lightDurationRange = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{5, minStr},
            NumWithStringListItem::Value{10, minStr},
            NumWithStringListItem::Value{15, minStr}};
        auto lightDuration = new NumWithStringListItem(
            model.getLightDuration(),
            lightDurationRange,
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_bottom_description"));

        lightDuration->onReturn = [chimeDuration, this]() {
            if (chimeDuration->isOff()) {
                list->rebuildList(gui::listview::RebuildType::OnOffset, 0);
                return true;
            }
            return false;
        };

        internalData.emplace_back(lightDuration);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }
} // namespace app::bell_settings
