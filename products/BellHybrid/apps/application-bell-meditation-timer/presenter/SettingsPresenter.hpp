// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"
#include <common/BellListItemProvider.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    namespace models
    {
        class ChimeInterval;
        class ChimeVolume;
        class StartDelay;
    } // namespace models

    class SettingsPresenter : public contract::Presenter
    {
      public:
        SettingsPresenter(models::ChimeInterval &chimeIntervalModel,
                          models::ChimeVolume &chimeVolumeModel,
                          models::StartDelay &startDelayModel,
                          AbstractAudioModel &audioModel,
                          AbstractAlarmModel &alarm);
        void loadData() override;
        void saveData() override;
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void exitWithSave() override;
        void exitWithoutSave() override;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      private:
        void stopSound();
        models::ChimeInterval &chimeIntervalModel;
        models::ChimeVolume &chimeVolumeModel;
        models::StartDelay &startDelayModel;
        AbstractAudioModel &audioModel;
        std::shared_ptr<BellListItemProvider> listItemsProvider;
        AbstractAlarmModel &alarmModel;
    };
} // namespace app::meditation
