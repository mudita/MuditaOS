// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <json11.hpp>
#include <utility>

namespace app::onBoarding
{

    class OnBoardingSwitchData : public gui::SwitchData
    {
      public:
        OnBoardingSwitchData() : SwitchData("onBoardingSwitchData")
        {}
    };
} // namespace app::onBoarding
