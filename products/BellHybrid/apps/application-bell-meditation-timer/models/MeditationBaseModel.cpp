// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationBaseModel.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace app::meditation
{
    auto MeditationBaseModel::createData() -> void
    {
        meditationItem = new MeditationItem();
    }

    auto MeditationBaseModel::setData(MeditationItem &item) -> void
    {
        if (meditationItem == nullptr) {
            meditationItem = new MeditationItem();
        }
        meditationItem->copyFrom(&item);
    }

    auto MeditationBaseModel::getData() -> MeditationItem *
    {
        return meditationItem;
    }
} // namespace app::meditation
