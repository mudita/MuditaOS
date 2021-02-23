// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>
#include <module-utils/common_data/EventStore.hpp>

namespace app::manager::actions
{
    /** Action parameters for
     * RequestScreenPasscode
     */
    class ScreenPasscodeParams : public ActionParams
    {
        bool cancel           = false;
        unsigned int attempts = 0;
        std::string passcodeName;

      public:
        ScreenPasscodeParams(bool cancel, unsigned int _attempts = 0, std::string _passcodeName = "Screen");

        [[nodiscard]] unsigned int getAttempts() const noexcept;
        [[nodiscard]] const std::string &getPasscodeName() const noexcept;
        [[nodiscard]] bool isCancel() const noexcept;
    };
} // namespace app::manager::actions
