// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>

#include "IntervalChimeModel.hpp"

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class IntervalChimeContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<IntervalChimeContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                        = default;
            virtual auto getProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual void loadIntervalList()                                      = 0;
            virtual void activate()                                              = 0;
        };
    };

    class IntervalChimePresenter : public IntervalChimeContract::Presenter
    {
      public:
        IntervalChimePresenter(app::ApplicationCommon *app, settings::Settings *settings);

        auto getProvider() -> std::shared_ptr<gui::ListItemProvider> override;
        void loadIntervalList() override;
        void activate() override;

      private:
        app::ApplicationCommon *app  = nullptr;
        settings::Settings *settings = nullptr;
        std::shared_ptr<IntervalChimeModel> model;
    };
} // namespace app::meditation
