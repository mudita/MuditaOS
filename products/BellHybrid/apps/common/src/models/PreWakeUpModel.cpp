// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/PreWakeUpModel.hpp"

namespace app
{
    bool PreWakeUpModel::isActive() const
    {
        return activity;
    }

    void PreWakeUpModel::setActive(bool active)
    {
        activity = active;
    }
} // namespace app
