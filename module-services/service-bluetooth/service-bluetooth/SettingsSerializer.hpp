// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json11.hpp>
#include <Device.hpp>

class SettingsSerializer
{
  public:
    static auto toString(const std::vector<Devicei> &devices) -> std::string;
    static auto fromString(const std::string &jsonStr) -> std::vector<Devicei>;
};
