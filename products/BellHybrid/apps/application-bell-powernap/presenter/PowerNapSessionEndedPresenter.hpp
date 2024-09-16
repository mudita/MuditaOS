// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
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
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit PowerNapSessionEndPresenter(app::ApplicationCommon *app);
    };
} // namespace app::powernap
