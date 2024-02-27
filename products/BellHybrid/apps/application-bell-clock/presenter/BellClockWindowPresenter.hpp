// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class AppWindow;
    class ListItemProvider;
} // namespace gui

namespace app::bell_clock
{
    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void exit()      = 0;
    };

    class AbstractClockPresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractClockPresenter() noexcept = default;
    };

    class BellClockWindowPresenter : public AbstractClockPresenter
    {
      public:
        explicit BellClockWindowPresenter();
    };
} // namespace app::bell_clock
