// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Settings.hpp"

#include <Service/Service.hpp>
#include <Utils.hpp>

namespace cellular::service
{
    namespace
    {
        constexpr auto activeSim = "gs_active_sim";
    }

    Settings::Settings(sys::Service *owner)
    {
        init(::service::ServiceProxy(owner->weak_from_this()));
    }

    void Settings::setSimSlot(api::SimSlot slot)
    {
        setValue(activeSim, utils::enumToString(slot));
    }

    std::optional<api::SimSlot> Settings::getSimSlot()
    {
        return magic_enum::enum_cast<api::SimSlot>(getValue(activeSim));
    }

} // namespace cellular::service
