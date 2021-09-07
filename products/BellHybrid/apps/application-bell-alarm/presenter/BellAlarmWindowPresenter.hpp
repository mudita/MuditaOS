
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app::bell_alarm
{
    class BellAlarmWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<BellAlarmWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept   = default;
            virtual auto saveData() -> void = 0;
        };
    };

    class BellAlarmWindowPresenter : public BellAlarmWindowContract::Presenter
    {
      public:
        auto saveData() -> void override;
    };
} // namespace app::bell_alarm
