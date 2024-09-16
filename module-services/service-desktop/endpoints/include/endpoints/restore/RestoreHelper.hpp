// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{
    class RestoreHelper : public BaseHelper
    {
      public:
        explicit RestoreHelper(sys::Service *p) : BaseHelper(p)
        {}

        auto processPost(Context &context) -> ProcessResult final;
    };
} // namespace sdesktop::endpoints
