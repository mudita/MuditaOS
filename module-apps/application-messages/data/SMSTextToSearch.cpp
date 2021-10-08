// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTextToSearch.hpp"

SMSTextToSearch::SMSTextToSearch(const std::string text_to_search, const std::string title)
    : text_to_search(text_to_search), title(title)
{}

auto SMSTextToSearch::getTextToSearch() const -> std::string
{
    return text_to_search;
}
