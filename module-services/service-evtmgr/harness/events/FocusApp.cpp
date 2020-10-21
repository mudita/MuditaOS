// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusApp.hpp"

#include <json/json11.hpp> // for Json, Json::object, json11

#include <map>    // for map<>::mapped_type
#include <vector> // for vector

namespace harness
{

    using namespace json11;

    std::string FocusApp::encode()
    {
        json11::Json::object el;
        el["t"] = (int)evt;
        el["d"] = std::vector<std::string>({name});
        return Json(el).dump();
    }

} // namespace harness
