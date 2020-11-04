// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GUIDisplayReady.hpp"
#include <json/json11.hpp>
#include "magic_enum.hpp"

namespace service::gui
{

    GUIDisplayReady::operator std::string() const
    {
        json11::Json::object vals;
        vals["in_suspend"]  = suspendInProgress ? "yes" : "no";
        vals["in_shutdown"] = shutdownInProgress ? "yes" : "no";
        return json11::Json(vals).dump();
    }
} // namespace service::gui
