// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsPopupQueue.hpp"
#include "WindowsPopupFilter.hpp"

namespace app
{
    std::optional<gui::popup::Request> WindowsPopupQueue::popRequest(const gui::popup::Filter &filter)
    {
        for (const auto &val : requests) {
            if (filter.isPermitted(val.getPopupParams())) {
                return {std::move(requests.extract(val).value())};
            }
        }
        return {std::nullopt};
    }

    void WindowsPopupQueue::pushRequest(gui::popup::Request &&r)
    {
        requests.emplace(std::move(r));
    }
} // namespace app
