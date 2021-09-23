// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>

#include "MeditationProgressModel.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class Item;
}

namespace app::meditation
{
    class MeditationProgressContract
    {
      public:
        class View
        {
          public:
            virtual ~View()                 = default;
            virtual void pregressFinished() = 0;
            virtual void intervalReached()  = 0;
            virtual void baseTickReached()  = 0;
        };
        class Presenter : public BasePresenter<MeditationProgressContract::View>
        {
          public:
            virtual void activate(MeditationItem &item)                            = 0;
            virtual void request(MeditationItem &item)                             = 0;
            virtual void initTimer(gui::Item *parent)                              = 0;
            virtual app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept = 0;
            virtual void start()                                                   = 0;
            virtual void stop()                                                    = 0;
            virtual void pause()                                                   = 0;
            virtual void resume()                                                  = 0;
        };
    };

    class MeditationProgressPresenter : public MeditationProgressContract::Presenter
    {
      private:
        app::ApplicationCommon *app = nullptr;
        std::unique_ptr<app::ProgressTimer> timer;
        std::shared_ptr<MeditationProgressModel> model;

        void onProgressFinished();
        void onIntervalReached();
        void onBaseTickReached();

      public:
        explicit MeditationProgressPresenter(app::ApplicationCommon *app);

        void activate(MeditationItem &item) override;
        void request(MeditationItem &item) override;
        void initTimer(gui::Item *parent) override;
        app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept override;
        void start() override;
        void stop() override;
        void pause() override;
        void resume() override;
    };
} // namespace app::meditation
