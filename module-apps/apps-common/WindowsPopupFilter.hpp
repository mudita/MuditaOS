// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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

    enum class FilterType
    {
        Show = 0, // Show popup
        Ignore,   // Ignore popup but add to the popup's queue
        Remove    // Remove and don't add to the popup's queue
    };

    /// This filter class is used just to filter next popup to handle if any
    /// it can and should be overriden to filter out only desired ones at the time
    class Filter
    {
      private:
        std::list<std::function<FilterType(const gui::PopupRequestParams &)>> appDependentFilter;
        /// non-owning pointer to existing stack - @see attachWindowsStack()
        app::WindowsStack *stack = nullptr;

      public:
        virtual ~Filter() = default;

        void attachWindowsStack(app::WindowsStack *stack);
        void addAppDependentFilter(std::function<FilterType(const gui::PopupRequestParams &)> f);
        virtual bool isPermitted(const gui::PopupRequestParams &params) const;
        virtual bool addPopup(const gui::PopupRequestParams &params) const;
    };
} // namespace gui::popup
