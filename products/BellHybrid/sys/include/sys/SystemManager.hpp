// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SystemManager/SystemManagerCommon.hpp>

namespace sys
{
    class AlarmActivationStatusChangeRequest;

    class SystemManager : public SystemManagerCommon
    {
      public:
        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);

      private:
        auto InitHandler() -> ReturnCodes override;

        auto handleAlarmActivationStatusChangeRequest(AlarmActivationStatusChangeRequest *request) -> MessagePointer;

        void handleShutdown() override;

        const std::vector<std::string> &getWhiteListFor(WhiteListType type) override;
    };
} // namespace sys
