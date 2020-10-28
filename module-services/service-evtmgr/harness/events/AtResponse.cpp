// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AtResponse.hpp"

#include <json/json11.hpp> // for Json, Json::object
#include <map>             // for map<>::mapped_type

#include "service-evtmgr/harness/Events.hpp" // for Data, Type

namespace harness
{

    std::string AtResponse::encode()
    {
        json11::Json::object el;
        el[Type] = static_cast<int>(evt);
        el[Data] = resp;
        return json11::Json(el).dump();
    }

} // namespace harness
