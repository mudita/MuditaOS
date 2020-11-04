// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AppRefreshMessage.hpp"
#include <json/json11.hpp>
#include "magic_enum.hpp"

namespace app
{
    AppRefreshMessage::operator std::string() const
    {
        json11::Json::object vals;
        vals["Window"] = this->getWindowName();
        vals["Mode"]   = std::string(magic_enum::enum_name(this->getMode()));
        return json11::Json(vals).dump();
    }
} // namespace app
