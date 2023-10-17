// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/data/PopupRequestParamsBase.hpp"
#include <time/time_conversion.hpp>
#include <functional>

namespace gui::popup
{
    /// Blueprint to create popup window/handle popup action if there is no window when required
    using Blueprint = std::function<bool(gui::popup::ID, std::unique_ptr<gui::PopupRequestParams> &)>;

    /// sortable class to select holding all data to handle popup
    class Request
    {
        const gui::popup::ID id;
        std::unique_ptr<gui::PopupRequestParams> params;
        utils::time::Timestamp timeRequested;
        Blueprint blueprint;

      public:
        Request(gui::popup::ID id, std::unique_ptr<gui::PopupRequestParams> &&params, Blueprint blueprint);
        /// with no Blueprint attached - will raise std::bad_function_call
        bool handle();
        bool operator<(const Request &p) const;
        const gui::PopupRequestParams &getPopupParams() const;
    };
} // namespace gui::popup
