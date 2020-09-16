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
