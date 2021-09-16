// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/TimeModel.hpp"

namespace app
{

    time_t TimeModel::getCurrentTime() const
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }
    utils::time::Locale::TimeFormat TimeModel::getTimeFormat() const
    {
        return stm::api::timeFormat();
    }
} // namespace app
