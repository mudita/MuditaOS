// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::relaxation
{
    class RelaxationErrorContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<RelaxationErrorContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class RelaxationErrorPresenter : public RelaxationErrorContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit RelaxationErrorPresenter(app::ApplicationCommon *app);
    };
} // namespace app::relaxation
