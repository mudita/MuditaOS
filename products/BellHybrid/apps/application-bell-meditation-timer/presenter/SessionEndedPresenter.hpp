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
    class SessionEndedPresenterContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<SessionEndedPresenterContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class SessionEndedPresenter : public SessionEndedPresenterContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit SessionEndedPresenter(app::ApplicationCommon *app);
    };
} // namespace app::meditation
