// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

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
            virtual std::uint32_t getMinValue()                      = 0;
            virtual std::uint32_t getMaxValue()                      = 0;
            virtual std::uint32_t getStepValue()                     = 0;
            virtual std::uint32_t getCurrentValue()                  = 0;
            virtual std::string getTimeUnitName(std::uint32_t value) = 0;
            virtual void activate(std::uint32_t value)               = 0;
        };
    };

    class MeditationTimerPresenter : public MeditationTimerContract::Presenter
    {
      public:
        MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings);

        std::uint32_t getMinValue() override;
        std::uint32_t getMaxValue() override;
        std::uint32_t getStepValue() override;
        std::uint32_t getCurrentValue() override;
        std::string getTimeUnitName(std::uint32_t value) override;
        void activate(std::uint32_t value) override;

      private:
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
    };
} // namespace app::meditation
