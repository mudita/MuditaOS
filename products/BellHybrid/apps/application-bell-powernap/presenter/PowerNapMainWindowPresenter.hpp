// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app::powernap
{
    class PowerNapMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<PowerNapMainWindowContract::View>
        {};
    };

    class PowerNapMainWindowPresenter : public PowerNapMainWindowContract::Presenter
    {};

} // namespace app::powernap
