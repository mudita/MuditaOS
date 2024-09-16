// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace settings
{
    class Settings;
}

namespace app::meditation
{
    class MeditationTimerContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<MeditationTimerContract::View>
        {
          public:
            virtual ~Presenter() noexcept                            = default;
            virtual std::uint8_t getMinValue()                       = 0;
            virtual std::uint8_t getMaxValue()                       = 0;
            virtual std::uint8_t getStepValue()                      = 0;
            virtual std::uint8_t getCurrentValue()                   = 0;
            virtual std::string getTimeUnitName(std::uint32_t value) = 0;
            virtual void activate(std::uint32_t value)               = 0;
        };
    };

    class MeditationTimerPresenter : public MeditationTimerContract::Presenter
    {
      public:
        MeditationTimerPresenter(app::ApplicationCommon *app,
                                 settings::Settings *settings,
                                 AbstractBatteryModel &batteryModel,
                                 AbstractLowBatteryInfoModel &lowBatteryInfoModel);

        std::uint8_t getMinValue() override;
        std::uint8_t getMaxValue() override;
        std::uint8_t getStepValue() override;
        std::uint8_t getCurrentValue() override;
        std::string getTimeUnitName(std::uint32_t value) override;
        void activate(std::uint32_t value) override;

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;
    };
} // namespace app::meditation
