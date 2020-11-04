// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManagerCmd.hpp"
#include <json/json11.hpp>
#include "magic_enum.hpp"

namespace sys
{

    SystemManagerCmd::operator std::string() const
    {
        json11::Json::object vals;
        vals["Code"] = std::string(magic_enum::enum_name(type));
        return json11::Json(vals).dump();
    }
} // namespace sys
