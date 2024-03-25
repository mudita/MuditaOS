// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsListItemProvider.hpp"
#include "SongsListViewItem.hpp"
#include <common/models/AbstractAudioModel.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>

namespace app::bell_settings
{
    using namespace gui;

    BedtimeSettingsListItemProvider::BedtimeSettingsListItemProvider(
        std::shared_ptr<AbstractBedtimeModel> settingsModel, std::shared_ptr<SongsModel> songsModel)
        : settingsModel{std::move(settingsModel)}, songsModel{std::move(songsModel)}
    {
        buildListItems();
    }

    void BedtimeSettingsListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 2U;
        internalData.reserve(itemCount);

        auto chimeTone = new SongsListViewItem(
            utils::translate("app_bell_settings_bedtime_settings_tone"), settingsModel->getBedtimeTone(), songsModel);

        currentSoundPath = settingsModel->getBedtimeTone().getValue();
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
        bedtimeVolume             = new list_items::NumericWithBar(
            list_items::NumericWithBar::spinner_type::range{volumeMin, volumeMax, volumeStep},
            settingsModel->getBedtimeVolume(),
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
