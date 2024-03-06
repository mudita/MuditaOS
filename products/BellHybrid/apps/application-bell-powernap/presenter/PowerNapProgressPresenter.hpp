// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
    class PowerNapFrontlightModel;

    class PowerNapProgressContract
    {
      public:
        class View
        {
          public:
            ~View()                                                         = default;
            virtual auto napEnded() -> void                                         = 0;
            virtual auto setTime(std::time_t newTime) -> void                       = 0;
            virtual auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void = 0;
            virtual auto progressFinished() -> void                                 = 0;
            virtual auto pause() -> void                                            = 0;
            virtual auto resume() -> void                                           = 0;
        };

        class Presenter : public BasePresenter<PowerNapProgressContract::View>
        {
          public:
            virtual auto activate() -> void                                                 = 0;
            virtual auto endNap() -> void                                                   = 0;
            virtual auto abortNap() -> void                                                 = 0;
            virtual auto isTimerStopped() -> bool                                           = 0;
            virtual auto pause() -> void                                                    = 0;
            virtual auto resume() -> void                                                   = 0;
            virtual auto setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) -> void = 0;
            virtual auto handleUpdateTimeEvent() -> void                                    = 0;
            virtual auto isNapFinished() -> bool                                            = 0;
            virtual auto onBeforeShow() -> void                                             = 0;
        };
    };

    class PowerNapProgressPresenter : public PowerNapProgressContract::Presenter
    {
      public:
        PowerNapProgressPresenter(app::ApplicationCommon *app,
                                  settings::Settings *settings,
                                  AbstractAudioModel &audioModel,
                                  std::unique_ptr<AbstractTimeModel> timeModel,
                                  std::unique_ptr<PowerNapFrontlightModel> frontlightModel,
                                  const std::chrono::seconds &powerNapAlarmDuration);

        auto activate() -> void override;
        auto endNap() -> void override;
        auto abortNap() -> void override;
        auto pause() -> void override;
        auto resume() -> void override;
        auto setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) -> void override;
        auto handleUpdateTimeEvent() -> void override;
        auto isNapFinished() -> bool override;
        auto isTimerStopped() -> bool override;

        auto onNapFinished() -> void;
        auto onNapAlarmFinished() -> void;
        auto onBeforeShow() -> void override;

      private:
        static constexpr auto endWindowTimeout = std::chrono::seconds{5};

        app::ApplicationCommon *app{};
        settings::Settings *settings{};
        AbstractAudioModel &audioModel;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::unique_ptr<AbstractTimeModel> timeModel;
        std::unique_ptr<PowerNapFrontlightModel> frontlightModel;
        sys::TimerHandle napAlarmTimer;
        bool napFinished{false};
    };
} // namespace app::powernap
