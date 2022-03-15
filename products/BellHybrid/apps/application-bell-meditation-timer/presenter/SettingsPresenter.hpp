// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"
#include <common/BellListItemProvider.hpp>
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
        SettingsPresenter(app::ApplicationCommon *app,
                          models::ChimeInterval &chimeIntervalModel,
                          models::ChimeVolume &chimeVolumeModel,
                          models::StartDelay &startDelayModel);
        void loadData() override;
        void saveData() override;
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void handleEnter() override;

      private:
        ApplicationCommon *application{};
        models::ChimeInterval &chimeIntervalModel;
        models::ChimeVolume &chimeVolumeModel;
        models::StartDelay &startDelayModel;
        std::shared_ptr<BellListItemProvider> listItemsProvider;
    };
} // namespace app::meditation
