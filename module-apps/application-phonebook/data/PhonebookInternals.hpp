#pragma once

namespace phonebookInternals
{
    constexpr uint8_t speedDialMaxValue = 9;

    enum class ListItemName
    {
        FirstName,
        SecondName,
        Number,
        OtherNumber,
        Email,
        SpeedDialKey,
        AddToFavourites,
        AddToICE,
        Address,
        Note
    };
} // namespace phonebookInternals
