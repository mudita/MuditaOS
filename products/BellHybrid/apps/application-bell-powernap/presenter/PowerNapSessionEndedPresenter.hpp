// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app
{
    class Application;
}

namespace app::powernap
{
    class PowerNapSessionEndedContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<PowerNapSessionEndedContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class PowerNapSessionEndPresenter : public PowerNapSessionEndedContract::Presenter
    {
        app::Application *app{};
        void activate() override;

      public:
        explicit PowerNapSessionEndPresenter(app::Application *app);
    };
} // namespace app::powernap
