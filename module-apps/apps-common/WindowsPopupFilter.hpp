// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <list>

namespace gui
{
    class PopupRequestParams;
}
namespace app
{
    class WindowsStack;
}
namespace gui::popup
{
    /// This filter class is used just to filter next popup to handle if any
    /// it can and should be overriden to filter out only desired ones at the time
    class Filter
    {
      private:
        std::list<std::function<bool(const gui::PopupRequestParams &)>> appDependentFilter;
        /// non-owning pointer to existing stack - @see attachWindowsStack()
        app::WindowsStack *stack = nullptr;

      public:
        virtual ~Filter() = default;

        void attachWindowsStack(app::WindowsStack *stack);
        void addAppDependentFilter(std::function<bool(const gui::PopupRequestParams &)> f);
        virtual bool isPermitted(const gui::PopupRequestParams &params) const;
    };
} // namespace gui::popup
