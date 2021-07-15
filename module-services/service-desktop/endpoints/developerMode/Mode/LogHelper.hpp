// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <endpoints/BaseHelper.hpp>

namespace parserFSM
{

    class LogHelper : public BaseHelper
    {

      public:
        explicit LogHelper(const std::string &name, sys::Service *p) : BaseHelper(name, p)
        {}

        auto processPost(Context &context) -> ProcessResult final;
        auto processGet(Context &context) -> ProcessResult final;

      private:
        [[nodiscard]] auto setLogLevel(const std::string &, double) -> ProcessResult;
        [[nodiscard]] auto setLogDevice(const std::string &dev) -> ProcessResult;
        [[nodiscard]] auto getLogLevel(const std::string &) -> ProcessResult;
    };
} // namespace parserFSM
