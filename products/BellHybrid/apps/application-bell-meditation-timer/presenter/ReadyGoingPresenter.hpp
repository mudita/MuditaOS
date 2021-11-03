// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::meditation
{
    class ReadyGoingPresenterContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<ReadyGoingPresenterContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class ReadyGoingPresenter : public ReadyGoingPresenterContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit ReadyGoingPresenter(app::ApplicationCommon *app);
    };
} // namespace app::meditation
