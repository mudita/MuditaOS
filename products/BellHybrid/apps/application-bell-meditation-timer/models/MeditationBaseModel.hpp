// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationItem.hpp"

#include <functional>
#include <string>

namespace app::meditation
{
    class MeditationBaseModel
    {
      protected:
        MeditationItem *meditationItem{nullptr};

      public:
        auto createData() -> void;
        auto setData(MeditationItem &item) -> void;
        auto getData() -> MeditationItem *;
    };
} // namespace app::meditation
