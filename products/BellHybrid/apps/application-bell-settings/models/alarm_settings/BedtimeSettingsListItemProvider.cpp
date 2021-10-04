// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsListItemProvider.hpp"
#include "BellSettingsStyle.hpp"
#include <common/widgets/ListItems.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    BedtimeSettingsListItemProvider::BedtimeSettingsListItemProvider(std::shared_ptr<AbstractBedtimeModel> model,
                                                                     std::vector<UTF8> chimeToneRange)
        : model{model}
    {
        buildListItems(std::move(chimeToneRange));
    }

    void BedtimeSettingsListItemProvider::buildListItems(std::vector<UTF8> chimeToneRange)
    {
        constexpr auto itemCount = 2U;
        internalData.reserve(itemCount);

        auto chimeTone = new UTF8ListItem(model.get()->getBedtimeTone(),
                                          std::move(chimeToneRange),
                                          utils::translate("app_bell_settings_bedtime_settings_tone"));
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
        auto volume               = new NumListItem(model.get()->getBedtimeVolume(),
                                      UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                                      utils::translate("app_bell_settings_bedtime_settings_volume"));
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
        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

} // namespace app::bell_settings
