// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app::focus
{
    class FocusTimerContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept = default;
        };
    };

    class FocusTimerPresenter : public FocusTimerContract::Presenter
    {
      public:
        FocusTimerPresenter(app::ApplicationCommon *app,
                            settings::Settings *settings,
                            AbstractBatteryModel &batteryModel,
                            AbstractLowBatteryInfoModel &lowBatteryInfoModel);

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;
    };
} // namespace app::focus
