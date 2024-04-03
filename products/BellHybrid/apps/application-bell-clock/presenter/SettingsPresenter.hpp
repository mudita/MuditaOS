// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"
#include <common/BellListItemProvider.hpp>
#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::clock
{
    namespace models
    {
        class DeepRefreshInterval;
    } // namespace models

    class SettingsPresenter : public contract::Presenter
    {
      public:
        SettingsPresenter(models::DeepRefreshInterval &startDelayModel);
        void loadData() override;
        void saveData() override;
        std::shared_ptr<gui::ListItemProvider> getPagesProvider() const override;
        void eraseProviderData() override;
        void exitWithSave() override;
        void exitWithoutSave() override;

      private:
        models::DeepRefreshInterval &startDelayModel;
        std::shared_ptr<BellListItemProvider> listItemsProvider;
    };
} // namespace app::clock
