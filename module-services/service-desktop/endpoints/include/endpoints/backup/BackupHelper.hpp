// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

namespace sdesktop::endpoints
{
    class BackupHelper : public BaseHelper
    {
      public:
        explicit BackupHelper(sys::Service *p) : BaseHelper(p)
        {}

        auto processPost(Context &context) -> ProcessResult final;
        auto processGet(Context &context) -> ProcessResult final;

      private:
        auto executeRequest(Context &context) -> ProcessResult;
        auto checkState(Context &context) -> ProcessResult;
        auto executeSyncRequest(Context &context) -> ProcessResult;
        auto checkSyncState(Context &context) -> ProcessResult;
    };
} // namespace sdesktop::endpoints
