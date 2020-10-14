/*
 * Common.cpp
 *
 *  Created on: 8 maj 2019
 *      Author: robert
 */
#include <cstdint>
#include <type_traits>
#include "Common.hpp"

namespace gui
{

    static timeSecondsFunctionPtr timeSecondsFunction = nullptr;

    bool operator&(const gui::RectangleEdge &lhs, const gui::RectangleEdge &rhs)
    {
        using T = std::underlying_type_t<RectangleEdge>;
        return static_cast<bool>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }

    RectangleEdge operator|(const RectangleEdge &lhs, const RectangleEdge &rhs)
    {
        using T = std::underlying_type_t<RectangleEdge>;
        return static_cast<RectangleEdge>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    RectangleEdge operator|=(RectangleEdge &lhs, const RectangleEdge &rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    uint32_t getTime()
    {
        if (timeSecondsFunction != nullptr)
            return timeSecondsFunction();
        return 0;
    }

    void setTimeFunction(timeSecondsFunctionPtr fptr)
    {
        timeSecondsFunction = fptr;
    }

} // namespace gui
