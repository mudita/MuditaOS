// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//
// Created by mateusz on 06.02.2020.
//
#pragma once

namespace gui
{
    enum class Axis
    {
        X,
        Y
    };

    inline Axis orthogonal(Axis axis)
    {
        if (axis == Axis::X) {
            return Axis::Y;
        }
        else {
            return Axis::X;
        }
    }
} // namespace gui
