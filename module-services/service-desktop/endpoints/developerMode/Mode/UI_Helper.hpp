// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <endpoints/BaseHelper.hpp>

namespace parserFSM
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
} // namespace parserFSM
