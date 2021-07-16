// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class SetOsUpdateVersion : public BaseMessage
    {
      public:
        explicit SetOsUpdateVersion(const ApplicationName &sender,
                                    const std::string &osUpdateVer,
                                    const std::string &osCurrentVer);
        std::string osUpdateVer{};
        std::string osCurrentVer{};
    };

} // namespace app::manager
