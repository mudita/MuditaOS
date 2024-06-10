// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::whatsNew
{
    class WhatsNewContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<WhatsNewContract::View>
        {
          public:
            virtual ~Presenter()                                               = default;
            virtual Store::Battery getBatteryState()                           = 0;
            virtual bool isBatteryCharging(Store::Battery::State state) const  = 0;
            virtual bool isBatteryBelowLowLevelThreshold(units::SOC soc) const = 0;
            [[nodiscard]] virtual bool isLowBatteryWindowHandled() const       = 0;
            virtual void handleLowBatteryWindow()                              = 0;
        };
    };

    class WhatsNewPresenter : public WhatsNewContract::Presenter
    {
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;

        Store::Battery getBatteryState() override;
        [[nodiscard]] bool isBatteryCharging(Store::Battery::State state) const override;
        [[nodiscard]] bool isBatteryBelowLowLevelThreshold(units::SOC soc) const override;
        [[nodiscard]] bool isLowBatteryWindowHandled() const override;
        void handleLowBatteryWindow() override;

      public:
        WhatsNewPresenter(AbstractBatteryModel &batteryModel, AbstractLowBatteryInfoModel &lowBatteryInfoModel);
    };
} // namespace app::whatsNew
