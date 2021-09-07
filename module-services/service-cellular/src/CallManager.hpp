// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace cellular::internal
{
    class SimpleCallManager
    {
      public:
        void hangUp() noexcept;
        void ring() noexcept;
        void completeCallerInfo() noexcept;

        [[nodiscard]] auto isIncomingCallPropagated() const noexcept -> bool;
        [[nodiscard]] auto isCallerInfoComplete() const noexcept -> bool;

      private:
        bool incomingCallPropagated{false};
        bool callerInfoComplete{false};
    };
} // namespace cellular::internal
