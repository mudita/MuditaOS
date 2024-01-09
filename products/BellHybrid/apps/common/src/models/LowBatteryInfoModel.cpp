// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/LowBatteryInfoModel.hpp"

namespace app
{
    bool LowBatteryInfoModel::isInfoHandled() const
    {
        return lowBatteryInfoHandled;
    }

    void LowBatteryInfoModel::handleInfo()
    {
        lowBatteryInfoHandled = true;
    }

} // namespace app
