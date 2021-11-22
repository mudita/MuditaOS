// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <common/models/AudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <time/time_locale.hpp>
#include <Timers/TimerHandle.hpp>
#include <memory>
namespace app
{
    class AbstractTimeModel;
    class ApplicationCommon;
} // namespace app
namespace gui
{
    class Item;
} // namespace gui
namespace settings
{
    class Settings;
}

namespace app::powernap
{
    class PowerNapProgressContract
    {
      public:
        class View
        {
          public:
            ~View()                                                         = default;
            virtual void napEnded()                                         = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        };

        class Presenter : public BasePresenter<PowerNapProgressContract::View>
        {
          public:
            virtual void activate()                                                 = 0;
            virtual void endNap()                                                   = 0;
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) = 0;
            virtual void handleUpdateTimeEvent()                                    = 0;
            virtual bool isNapFinished()                                            = 0;
        };
    };

    class PowerNapProgressPresenter : public PowerNapProgressContract::Presenter
    {
        app::ApplicationCommon *app{};
        settings::Settings *settings{};
        std::unique_ptr<AbstractSoundsRepository> soundsRepository;
        AbstractAudioModel &audioModel;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::unique_ptr<AbstractTimeModel> timeModel;
        sys::TimerHandle napAlarmTimer;
        bool napFinished{false};

        void activate() override;
        void endNap() override;
        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void handleUpdateTimeEvent() override;
        bool isNapFinished() override;

        void onNapFinished();
        void onNapAlarmFinished();

      public:
        PowerNapProgressPresenter(app::ApplicationCommon *app,
                                  settings::Settings *settings,
                                  std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                                  AbstractAudioModel &audioModel,
                                  std::unique_ptr<AbstractTimeModel> timeModel);
    };
} // namespace app::powernap
