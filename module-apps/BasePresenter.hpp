// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cassert>

namespace app
{
    template <typename VIEW> class BasePresenter
    {
      public:
        virtual ~BasePresenter() noexcept = default;

        void attach(VIEW *oth) noexcept
        {
            view = oth;
        }

        VIEW *getView() const noexcept
        {
            assert(view != nullptr);
            return view;
        }

      private:
        VIEW *view = nullptr;
    };
} // namespace app
