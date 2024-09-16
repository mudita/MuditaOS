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
    class PowerNapWarningContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<PowerNapWarningContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class PowerNapWarningPresenter : public PowerNapWarningContract::Presenter
    {
        app::ApplicationCommon *app{nullptr};
        void activate() override;

      public:
        explicit PowerNapWarningPresenter(app::ApplicationCommon *app);
    };
} // namespace app::powernap
