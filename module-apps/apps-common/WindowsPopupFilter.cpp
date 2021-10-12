// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsPopupFilter.hpp"
#include "WindowsStack.hpp"
#include "data/PopupRequestParams.hpp"
#include "AppWindow.hpp"

namespace gui::popup
{
    void Filter::attachWindowsStack(app::WindowsStack *stack)
    {
        this->stack = stack;
    }

    bool Filter::isPermitted(const gui::PopupRequestParams &params) const
    {
        for (const auto &filter : appDependentFilter) {
            if (filter != nullptr && not filter(params)) {
                return false;
            }
        }
        if (stack != nullptr) {
            /// there is no window to switch to
            auto data = stack->getWindowData(app::topWindow);
            if (not data) {
                return true;
            }
            if ((*data).disposition.id == params.getPopupId()) {
                /// its ok to switch to popup of the same ID
                /// this is actually used by onboarding
                return true;
            }
            if ((*data).disposition.priority > params.getDisposition().priority) {
                /// it's not ok to switch to popup of lesser priority
                /// equal ones are fine
                return false;
            }
        }
        return true;
    }

    void Filter::addAppDependentFilter(std::function<bool(const gui::PopupRequestParams &)> f)
    {
        appDependentFilter.push_back(f);
    }

} // namespace gui::popup
