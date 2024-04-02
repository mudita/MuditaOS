// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusTimerPresenter.hpp"

namespace app::focus
{
    FocusTimerPresenter::FocusTimerPresenter(app::ApplicationCommon *app,
                                             settings::Settings *settings,
                                             AbstractBatteryModel &batteryModel,
                                             AbstractLowBatteryInfoModel &lowBatteryInfoModel)
        : app{app}, settings{settings}, batteryModel{batteryModel}, lowBatteryInfoModel{lowBatteryInfoModel}
    {}

} // namespace app::focus
