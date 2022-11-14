// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::relaxation
{
    class RelaxationLowBatteryContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<RelaxationLowBatteryContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class RelaxationLowBatteryPresenter : public RelaxationLowBatteryContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit RelaxationLowBatteryPresenter(app::ApplicationCommon *app);
    };
} // namespace app::relaxation
