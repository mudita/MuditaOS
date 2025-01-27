// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{
    class QuotesHelper : public BaseHelper
    {
      public:
        explicit QuotesHelper(sys::Service *p) : BaseHelper(p)
        {}

        auto processGet(Context &context) -> ProcessResult final;
        auto processPost(Context &context) -> ProcessResult final;
        auto processPut(Context &context) -> ProcessResult final;
        auto processDelete(Context &context) -> ProcessResult final;
    };
} // namespace sdesktop::endpoints
