// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>

class SMSTextToSearch : public gui::SwitchData
{
    std::string text_to_search;

  public:
    SMSTextToSearch(const std::string text_to_search);
    [[nodiscard]] auto getTextToSearch() const -> std::string;
};
