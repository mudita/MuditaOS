// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <set>
#include <utility>
#include "WindowsPopupRequest.hpp"

namespace app
{
    class WindowsStack;
}
namespace gui
{
    namespace popup
    {
        class Filter;
    } // namespace popup
} // namespace gui

namespace app
{
    class WindowsPopupQueue
    {
      public:
        /// pops next available popup we require to show
        /// * in order set by Disposition
        /// * limited by Filter passed to request
        std::optional<gui::popup::Request> popRequest(const gui::popup::Filter &filter);

        /// append element to stack
        /// returns success/fail
        void pushRequest(gui::popup::Request &&r);

      private:
        /// set of requests to handle if there are more than one
        /// requests have to be sortable (this is by design)
        /// to iterate by Requests in priority order
        /// this behaviour can be easily changed - just change:
        /// - gui::popup::Disposition class to hold more data
        /// - change comparison algorithm in Request (please use std::set then)
        std::multiset<gui::popup::Request> requests{};
    };
} // namespace app
