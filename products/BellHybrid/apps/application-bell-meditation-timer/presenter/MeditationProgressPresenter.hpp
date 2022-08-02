// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <time/time_locale.hpp>

#include <memory>

namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app

namespace gui
{
    class Item;
}

namespace settings
{
    class Settings;
}

namespace app::meditation
{
    namespace models
    {
        class ChimeInterval;
        class Statistics;
    } // namespace models

    class MeditationProgressContract
    {
      public:
        class View
        {
          public:
            virtual ~View()                                                 = default;
            virtual void progressFinished()                                 = 0;
            virtual void intervalReached()                                  = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
            virtual void pause()                                            = 0;
            virtual void resume()                                           = 0;
        };
        class Presenter : public BasePresenter<MeditationProgressContract::View>
        {
          public:
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) = 0;
            virtual void handleUpdateTimeEvent()                                    = 0;
            virtual bool isTimerStopped()                                           = 0;
            virtual void start()                                                    = 0;
            virtual void stop()                                                     = 0;
            virtual void pause()                                                    = 0;
            virtual void resume()                                                   = 0;
            virtual void abandon()                                                  = 0;
            virtual void finish()                                                   = 0;
            virtual void onBeforeShow()                                             = 0;
        };
    };

    class MeditationProgressPresenter : public MeditationProgressContract::Presenter
    {
      private:
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::unique_ptr<AbstractTimeModel> timeModel;
        models::ChimeInterval &chimeIntervalModel;
        models::Statistics &statisticsModel;
        std::chrono::minutes duration;
        std::chrono::seconds interval;

        static constexpr auto endWindowTimeout = std::chrono::seconds{5};

        void onProgressFinished();
        void onIntervalReached();

        void addMeditationEntry(std::chrono::minutes elapsed);

      public:
        MeditationProgressPresenter(app::ApplicationCommon *app,
                                    settings::Settings *settings,
                                    std::unique_ptr<AbstractTimeModel> timeModel,
                                    models::ChimeInterval &chimeIntervalModel,
                                    models::Statistics &statisticsModel);

        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void handleUpdateTimeEvent() override;
        bool isTimerStopped() override;
        void start() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void abandon() override;
        void finish() override;
        void onBeforeShow() override;
    };
} // namespace app::meditation
