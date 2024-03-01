// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsListItemProvider.hpp"
#include "BellSettingsStyle.hpp"
#include "common/models/AbstractAudioModel.hpp"
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    BedtimeSettingsListItemProvider::BedtimeSettingsListItemProvider(std::shared_ptr<AbstractBedtimeModel> model,
                                                                     std::vector<UTF8> chimeToneRange)
        : model{std::move(model)}
    {
        buildListItems(std::move(chimeToneRange));
    }

    void BedtimeSettingsListItemProvider::buildListItems(std::vector<UTF8> chimeToneRange)
    {
        constexpr auto itemCount = 2U;
        internalData.reserve(itemCount);

        auto chimeTone = new list_items::Text(std::move(chimeToneRange),
                                              model->getBedtimeTone(),
                                              utils::translate("app_bell_settings_bedtime_settings_tone"));
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
        constexpr auto volumeMin  = AbstractAudioModel::minVolume;
        constexpr auto volumeMax  = AbstractAudioModel::maxVolume;
        bedtimeVolume             = new list_items::NumericWithBar(
            list_items::NumericWithBar::spinner_type::range{volumeMin, volumeMax, volumeStep},
            model->getBedtimeVolume(),
            volumeMax,
            utils::translate("app_bell_settings_bedtime_settings_volume"));
        bedtimeVolume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        bedtimeVolume->onEnter = [this, chimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(chimeTone->value());
            }
        };

        bedtimeVolume->onExit = [this]() {
            if (onVolumeExit) {
                onVolumeExit(bedtimeVolume->value());
            }
        };
        internalData.emplace_back(bedtimeVolume);
        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto BedtimeSettingsListItemProvider::getCurrentVolume() -> std::uint8_t
    {
        return bedtimeVolume->value();
    }
} // namespace app::bell_settings
