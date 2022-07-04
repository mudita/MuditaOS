// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/StartDelay.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <time/time_locale.hpp>

#include <memory>

namespace app
{
    class ApplicationCommon;
} // namespace app

namespace gui
{
    class Item;
}

namespace app::meditation
{
    class MeditationCountdownContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<MeditationCountdownContract::View>
        {
          public:
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) = 0;
            virtual void start()                                                    = 0;
            virtual void stop()                                                     = 0;
        };
    };

    class MeditationCountdownPresenter : public MeditationCountdownContract::Presenter
    {
      private:
        app::ApplicationCommon *app{nullptr};
        models::StartDelay &startDelayModel;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::chrono::seconds duration;

        void onCountdownFinished();

      public:
        MeditationCountdownPresenter(app::ApplicationCommon *app, models::StartDelay &startDelay);

        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void start() override;
        void stop() override;
    };
} // namespace app::meditation
