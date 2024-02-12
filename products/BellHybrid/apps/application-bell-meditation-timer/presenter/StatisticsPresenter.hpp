// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"
#include <common/models/AbstractAlarmModel.hpp>

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
        StatisticsPresenter(app::ApplicationCommon *app,
                            const models::Statistics &statisticsModel,
                            AbstractAlarmModel &alarm);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void handleExit() override;
        bool handleIfPreWakeupIsToTurnOffFirst() override;

      private:
        app::ApplicationCommon *app{nullptr};
        std::shared_ptr<BellListItemProvider> listItemsProvider;
        AbstractAlarmModel &alarmModel;
    };
} // namespace app::meditation
