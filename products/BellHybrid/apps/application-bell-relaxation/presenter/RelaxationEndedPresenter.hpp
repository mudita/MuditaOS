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
    class RelaxationEndedContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<RelaxationEndedContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class RelaxationEndedPresenter : public RelaxationEndedContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit RelaxationEndedPresenter(app::ApplicationCommon *app);
    };
} // namespace app::relaxation
