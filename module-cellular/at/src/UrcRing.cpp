// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <UrcRing.hpp>

auto at::urc::Ring::isValid() const noexcept -> bool
{
    if (getType() == std::nullopt) {
        return false;
    }
    return Urc::isValid();
}

auto at::urc::Ring::getType() const -> std::optional<at::urc::Ring::RingType>
{
    if (urcBody == at::urc::Ring::headNormal) {
        return at::urc::Ring::RingType::Normal;
    }
    else if (urcBody == "ASYNC") {
        return at::urc::Ring::RingType::Async;
    }
    else if (urcBody == "SYNC") {
        return at::urc::Ring::RingType::Sync;
    }
    else if (urcBody == "REL ASYNC") {
        return at::urc::Ring::RingType::RelAsync;
    }
    else if (urcBody == "REL SYNC") {
        return at::urc::Ring::RingType::RelSync;
    }
    else if (urcBody == "FAX") {
        return at::urc::Ring::RingType::Fax;
    }
    else if (urcBody == "VOICE") {
        return at::urc::Ring::RingType::Voice;
    }

    return std::nullopt;
}
