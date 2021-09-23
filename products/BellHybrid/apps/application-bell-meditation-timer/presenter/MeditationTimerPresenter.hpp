// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include "MeditationItem.hpp"
#include "MeditationTimerModel.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class MeditationTimerContract
    {
      public:
        class View
        {
          public:
            virtual ~View()              = default;
            virtual void updateDisplay() = 0;
        };

        class Presenter : public BasePresenter<MeditationTimerContract::View>
        {
          public:
            virtual ~Presenter() noexcept                = default;
            virtual void activate(MeditationItem &item)  = 0;
            virtual void request(MeditationItem &item)   = 0;
            virtual void increase()                      = 0;
            virtual void decrease()                      = 0;
            virtual auto getTimerString() -> std::string = 0;
        };
    };

    class MeditationTimerPresenter : public MeditationTimerContract::Presenter
    {
      public:
        explicit MeditationTimerPresenter(app::ApplicationCommon *app);

        void activate(MeditationItem &item) override;
        void request(MeditationItem &item) override;
        void increase() override;
        void decrease() override;
        auto getTimerString() -> std::string;

      private:
        app::ApplicationCommon *app = nullptr;
        std::shared_ptr<MeditationTimerModel> model;

        void updateDisplay();
    };
} // namespace app::meditation
