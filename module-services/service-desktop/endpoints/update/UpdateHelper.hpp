// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <endpoints/BaseHelper.hpp>

namespace parserFSM
{

    class UpdateHelper : public BaseHelper
    {
      public:
        explicit UpdateHelper(sys::Service *p) : BaseHelper(p)
        {}

        auto processPost(Context &context) -> ProcessResult final;
        void preProcess(http::Method method, Context &context) final;
    };
} // namespace parserFSM
