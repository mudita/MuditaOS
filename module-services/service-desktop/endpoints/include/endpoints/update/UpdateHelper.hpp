// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

#include <filesystem>

namespace sdesktop::endpoints
{
    class UpdateHelper : public BaseHelper
    {
      public:
        explicit UpdateHelper(sys::Service *p);

        auto processPost(Context &context) -> ProcessResult final;
        auto processPut(Context &context) -> ProcessResult final;
        void preProcess(http::Method method, Context &context) final;

      private:
        std::filesystem::path updatePackagePath;
        std::filesystem::path binariesPath;
    };
} // namespace sdesktop::endpoints
