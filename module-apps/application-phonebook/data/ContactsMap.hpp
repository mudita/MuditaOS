// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <map>

namespace phonebookContactsMap
{
    constexpr uint32_t NO_MATCH_FOUND = 0;
}

enum class ContactQuerySection
{
    Favourites = 0,
    Mixed,
};

enum class ContactDisplayMode
{
    Regular = 0,
    SortedByLetter,
};

enum class LabelMarkerDisplayMode
{
    IncludeFavourites = 0,
    IgnoreFavourites,
};

struct ContactsMapData
{
    std::map<std::string, std::uint32_t> firstLetterDictionary;
    std::uint32_t favouritesCount;
    std::uint32_t itemCount = 0;
};
