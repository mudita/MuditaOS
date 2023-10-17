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
    namespace models
    {
        class StartDelay;
    } // namespace models

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
            virtual void activate()                      = 0;
            virtual std::chrono::seconds getStartDelay() = 0;
        };
    };

    class ReadyGoingPresenter : public ReadyGoingPresenterContract::Presenter
    {
        app::ApplicationCommon *app{};
        const models::StartDelay &startDelayModel;
        void activate() override;
        std::chrono::seconds getStartDelay() override;

      public:
        ReadyGoingPresenter(app::ApplicationCommon *app, const models::StartDelay &startDelayModel);
    };
} // namespace app::meditation
