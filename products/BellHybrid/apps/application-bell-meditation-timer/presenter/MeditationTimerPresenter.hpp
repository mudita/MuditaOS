// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

#include "MeditationItem.hpp"
#include "MeditationTimerModel.hpp"

#include <memory>

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
            virtual ~Presenter() noexcept                                        = default;
            virtual void set(MeditationItem &item)                               = 0;
            virtual void get(MeditationItem &item)                               = 0;
            virtual auto getProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;
        };
    };

    class MeditationTimerPresenter : public MeditationTimerContract::Presenter
    {
      public:
        explicit MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings);

        void set(MeditationItem &item) override;
        void get(MeditationItem &item) override;
        auto getProvider() -> std::shared_ptr<gui::ListItemProvider>;

      private:
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
        std::shared_ptr<MeditationTimerModel> model;
    };
} // namespace app::meditation
