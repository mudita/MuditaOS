// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SysStart.hpp"
#include <json/json11.hpp>

namespace harness
{

    using namespace json11;

    std::string SysStart::encode()
    {
        json11::Json::object el;
        el["t"] = (int)evt;
        return Json(el).dump();
    }

}; // namespace harness
