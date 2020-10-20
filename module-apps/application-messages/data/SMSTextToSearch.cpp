// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTextToSearch.hpp"

SMSTextToSearch::SMSTextToSearch(const std::string text_to_search) : text_to_search(text_to_search)
{}

auto SMSTextToSearch::getTextToSearch() const -> std::string
{
    return text_to_search;
}
