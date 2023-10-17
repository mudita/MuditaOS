// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
    class BellListItemProvider;
} // namespace app

namespace app::meditation
{
    namespace models
    {
        class Statistics;
    }
    class StatisticsPresenter : public contract::StatisticsPresenter
    {
      public:
        StatisticsPresenter(app::ApplicationCommon *app, const models::Statistics &statisticsModel);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void handleExit() override;

      private:
        app::ApplicationCommon *app{nullptr};
        std::shared_ptr<BellListItemProvider> listItemsProvider;
    };
} // namespace app::meditation
