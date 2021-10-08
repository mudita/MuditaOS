// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace phonebookInternals
{
    constexpr uint8_t speedDialMaxValue = 9;

    enum class ListItemName
    {
        FirstName,
        SecondName,
        Number,
        SecondNumber,
        Email,
        SpeedDialKey,
        AddToFavourites,
        AddToICE,
        Address,
        Note
    };
} // namespace phonebookInternals
