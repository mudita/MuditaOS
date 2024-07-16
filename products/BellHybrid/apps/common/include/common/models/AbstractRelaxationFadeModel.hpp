// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common/models/AbstractSettingsModel.hpp"

namespace app
{
    class AbstractRelaxationFadeModel
    {
      public:
        virtual ~AbstractRelaxationFadeModel()                       = default;
        virtual auto getFade() -> gui::AbstractSettingsModel<bool> & = 0;
    };
} // namespace app
