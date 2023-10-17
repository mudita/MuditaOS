// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>

class SMSTextToSearch : public gui::SwitchData
{
    std::string text_to_search;
    std::string title;

  public:
    SMSTextToSearch(const std::string text_to_search, const std::string title);
    [[nodiscard]] auto getTextToSearch() const -> std::string;
    [[nodiscard]] auto getTitle() const -> std::string
    {
        return title;
    }
};
