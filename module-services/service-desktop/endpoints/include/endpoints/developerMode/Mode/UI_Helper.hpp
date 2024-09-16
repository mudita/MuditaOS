// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{

    class UI_Helper : public BaseHelper
    {

      public:
        explicit UI_Helper(sys::Service *p) : BaseHelper(p)
        {}

        /// returns DOM async
        auto processGet(Context &context) -> ProcessResult final;
        void preProcess(http::Method method, Context &context) final;
    };

} // namespace sdesktop::endpoints
